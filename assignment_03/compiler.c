#include <stdio.h>
#include <string.h>

#include "compiler.h"

#define MAX_LABELS 100

typedef struct
{
    char name[50];
    int address;
} Label;

Label labels[MAX_LABELS];
int label_count = 0;

int find_label(const char *name)
{
    for (int i = 0; i < label_count; i++)
    {
        if (strcmp(labels[i].name, name) == 0)
        {
            return labels[i].address;
        }
    }

    return -1;
}

void compile(const char *source_file, const char *program_file)
{
    label_count = 0;
    FILE *input;
    FILE *output;

    char line[100];

    input = fopen(source_file, "r");

    if (input == NULL)
    {
        printf("Error: Cannot open %s\n", source_file);
        return;
    }

    output = fopen(program_file, "w");

    if (output == NULL)
    {
        printf("Error: Cannot create %s\n", program_file);
        fclose(input);
        return;
    }

    /* ============================================================
     * FIRST PASS
     *
     * Find all labels and calculate their byte addresses.
     * ============================================================
     */

    FILE *pass1 = fopen(source_file, "r");

    if (pass1 == NULL)
    {
        printf("Error: Cannot open %s\n", source_file);
        return;
    }

    int address = 0;

    // char line[100];

    while (fgets(line, sizeof(line), pass1))
    {
        /* Remove comments */

        char *comment = strchr(line, '%');

        if (comment != NULL)
        {
            *comment = '\0';
        }

        /* Ignore blank lines */

        if (line[0] == '\0' || line[0] == '\n')
        {
            continue;
        }

        /* Check for label */

        if (line[0] == '.')
        {
            line[strcspn(line, "\r\n")] = '\0';

            if (label_count >= MAX_LABELS)
            {
                printf("Error: Too many labels.\n");
                fclose(pass1);
                return;
            }

            strcpy(labels[label_count].name, line);

            labels[label_count].address = address;

            printf("LABEL: %s -> address %d\n",
                   labels[label_count].name,
                   address);

            label_count++;

            continue;
        }

        /*
         * Every non-label instruction occupies
         * exactly four bytes.
         */

        address += 4;
    }

    fclose(pass1);

    int current_address = 0;
    while (fgets(line, sizeof(line), input))
    {
        int dest;
        int src1;
        int src2;
        int constant;

        char operation;

        /*
         * Remove comments.
         *
         * Anything after '%' is ignored.
         */

        char *comment = strchr(line, '%');

        if (comment != NULL)
        {
            *comment = '\0';
        }

        /*
         * Skip empty lines
         */

        if (line[0] == '\0' || line[0] == '\n')
        {
            continue;
        }

        /* Labels are handled in the first pass. They do not generate bytecode. */
        if (line[0] == '.')
        {
            continue;
        }


        /* =====================================================
           LEGACY READ

           Read x1, 0

           Opcode = 0x05
           ===================================================== */

        if (sscanf(line, "Read x%d, %d", &dest, &constant) == 2)
        {
            fprintf(output,
                    "%02X %02X %02X %02X\n",
                    0x0D,
                    dest,
                    0,
                    constant);
            current_address += 4;
            continue;
        }

        /* =====================================================
           LEGACY WRITE

           Write x1, 20

           Opcode = 0x06
           ===================================================== */

        if (sscanf(line, "Write x%d, %d", &dest, &constant) == 2)
        {
            fprintf(output,
                    "%02X %02X %02X %02X\n",
                    0x0E,
                    0,
                    dest,
                    constant);
            current_address += 4;
            continue;
        }

        /* ============================================================
         * MEMORY READ - VARIABLE ADDRESS
         *
         * x1 = [x2]
         *
         * Opcode: 0x05
         *
         * Bytecode:
         * 05 dest address_register 00
         * ============================================================
         */

        if (sscanf(line, "x%d = [x%d]", &dest, &src1) == 2)
        {
            fprintf(output,
                    "%02X %02X %02X %02X\n",
                    0x05,
                    dest,
                    src1,
                    0);
            current_address += 4;
            continue;
        }

        /* ============================================================
         * MEMORY READ - CONSTANT ADDRESS
         *
         * x1 = [100]
         *
         * Opcode: 0x0D
         *
         * Bytecode:
         * 0D dest 00 address
         * ============================================================
         */

        if (sscanf(line, "x%d = [%d]", &dest, &src1) == 2)
        {
            if (src1 < 0 || src1 > 255)
            {
                printf("Error: Memory address must be 0-255.\n");
                fclose(input);
                fclose(output);
                return;
            }

            fprintf(output,
                    "%02X %02X %02X %02X\n",
                    0x0D,
                    dest,
                    0,
                    src1);
            current_address += 4;
            continue;
        }

        /* ============================================================
         * MEMORY WRITE - VARIABLE ADDRESS
         *
         * [x1] = x2
         *
         * Opcode: 0x06
         *
         * Bytecode:
         * 06 00 value_register address_register
         * ============================================================
         */

        if (sscanf(line, "[x%d] = x%d", &dest, &src1) == 2)
        {
            fprintf(output,
                    "%02X %02X %02X %02X\n",
                    0x06,
                    0,
                    src1,
                    dest);

            current_address += 4;
            continue;
        }

        /* ============================================================
         * MEMORY WRITE - CONSTANT ADDRESS
         *
         * [100] = x2
         *
         * Opcode: 0x0E
         *
         * Bytecode:
         * 0E 00 value_register address
         * ============================================================
         */

        if (sscanf(line, "[%d] = x%d", &src1, &dest) == 2)
        {
            if (src1 < 0 || src1 > 255)
            {
                printf("Error: Memory address must be 0-255.\n");
                fclose(input);
                fclose(output);
                return;
            }

            fprintf(output,
                    "%02X %02X %02X %02X\n",
                    0x0E,
                    0,
                    dest,
                    src1);
            current_address += 4;
            continue;
        }

        /* ============================================================
         * VECTOR MEMORY READ
         *
         * v1 = [x2]     -> 8 consecutive 32-bit elements
         * v1 = [32]
         * ============================================================
         */
        if (sscanf(line, "v%d = [x%d]", &dest, &src1) == 2)
        {
            if (dest < 0 || dest > 31 || src1 < 0 || src1 > 255)
            {
                printf("Error: Invalid vector/register number.\n");
                fclose(input); fclose(output); return;
            }
            fprintf(output, "%02X %02X %02X %02X\n", 0x25, dest, src1, 0);
            current_address += 4;
            continue;
        }

        if (sscanf(line, "v%d = [%d]", &dest, &constant) == 2)
        {
            if (dest < 0 || dest > 31 || constant < 0 || constant > 255)
            {
                printf("Error: Invalid vector/address.\n");
                fclose(input); fclose(output); return;
            }
            fprintf(output, "%02X %02X %02X %02X\n", 0x2C, dest, 0, constant);
            current_address += 4;
            continue;
        }

        /* ============================================================
         * VECTOR MEMORY WRITE
         *
         * [x2] = v1
         * [32] = v1
         * ============================================================
         */
        if (sscanf(line, "[x%d] = v%d", &src1, &dest) == 2)
        {
            if (dest < 0 || dest > 31 || src1 < 0 || src1 > 255)
            {
                printf("Error: Invalid vector/register number.\n");
                fclose(input); fclose(output); return;
            }
            fprintf(output, "%02X %02X %02X %02X\n", 0x26, 0, dest, src1);
            current_address += 4;
            continue;
        }

        if (sscanf(line, "[%d] = v%d", &constant, &dest) == 2)
        {
            if (dest < 0 || dest > 31 || constant < 0 || constant > 255)
            {
                printf("Error: Invalid vector/address.\n");
                fclose(input); fclose(output); return;
            }
            fprintf(output, "%02X %02X %02X %02X\n", 0x2E, 0, dest, constant);
            current_address += 4;
            continue;
        }

        /* ============================================================
         * VECTOR ARITHMETIC
         *
         * v3 = v1 + v4
         * v3 = v1 + x4
         * v3 = v1 + 15
         * ============================================================
         */
        if (sscanf(line, "v%d = v%d %c v%d", &dest, &src1, &operation, &src2) == 4)
        {
            int vopcode = -1;
            if (operation == '+') vopcode = 0x21;
            else if (operation == '-') vopcode = 0x22;
            else if (operation == '*') vopcode = 0x23;
            if (vopcode < 0 || dest < 0 || dest > 31 || src1 < 0 || src1 > 31 || src2 < 0 || src2 > 31)
            {
                printf("Error: Invalid vector operation.\n");
                fclose(input); fclose(output); return;
            }
            fprintf(output, "%02X %02X %02X %02X\n", vopcode, dest, src1, src2);
            current_address += 4;
            continue;
        }

        if (sscanf(line, "v%d = v%d %c %d", &dest, &src1, &operation, &constant) == 4)
        {
            int vopcode = -1;
            if (operation == '+') vopcode = 0x29;
            else if (operation == '-') vopcode = 0x2A;
            else if (operation == '*') vopcode = 0x2B;
            if (vopcode < 0 || dest < 0 || dest > 31 || src1 < 0 || src1 > 31 || constant < 0 || constant > 255)
            {
                printf("Error: Invalid vector constant operation.\n");
                fclose(input); fclose(output); return;
            }
            fprintf(output, "%02X %02X %02X %02X\n", vopcode, dest, src1, constant);
            current_address += 4;
            continue;
        }

        /* =====================================================
           DATA MOVEMENT

           x1 = 100

           Opcode = 0x0F

           Format:

           opcode dest 0 constant

           0F 01 00 64
           ===================================================== */

        if (sscanf(line, "x%d = %d",
                   &dest,
                   &constant) == 2)
        {
            fprintf(output,
                    "%02X %02X %02X %02X\n",
                    0x0F,
                    dest,
                    0,
                    constant);

            current_address += 4;
            continue;
        }

        /* =====================================================
           ARITHMETIC WITH VARIABLE

           x1 = x2 + x3

           ADD = 01
           SUB = 02
           MUL = 03
           DIV = 04
           ===================================================== */

        if (sscanf(line,
                   "x%d = x%d %c x%d",
                   &dest,
                   &src1,
                   &operation,
                   &src2) == 4)
        {
            int opcode = 0;

            switch (operation)
            {
            case '+':
                opcode = 0x01;
                break;

            case '-':
                opcode = 0x02;
                break;

            case '*':
                opcode = 0x03;
                break;

            case '/':
                opcode = 0x04;
                break;

            default:
                printf(
                    "Error: Unsupported operator %c\n",
                    operation);

                fclose(input);
                fclose(output);

                return;
            }

            fprintf(output,
                    "%02X %02X %02X %02X\n",
                    opcode,
                    dest,
                    src1,
                    src2);
            current_address += 4;
            continue;
        }

        /* =====================================================
           ARITHMETIC WITH CONSTANT

           x1 = x2 + 10

           ADD constant = 09
           SUB constant = 0A
           MUL constant = 0B
           DIV constant = 0C
           ===================================================== */

        if (sscanf(line,
                   "x%d = x%d %c %d",
                   &dest,
                   &src1,
                   &operation,
                   &constant) == 4)
        {
            int opcode = 0;

            switch (operation)
            {
            case '+':
                opcode = 0x09;
                break;

            case '-':
                opcode = 0x0A;
                break;

            case '*':
                opcode = 0x0B;
                break;

            case '/':
                opcode = 0x0C;
                break;

            default:
                printf(
                    "Error: Unsupported operator %c\n",
                    operation);

                fclose(input);
                fclose(output);

                return;
            }

            fprintf(output,
                    "%02X %02X %02X %02X\n",
                    opcode,
                    dest,
                    src1,
                    constant);
            current_address += 4;
            continue;
        }

        /* ============================================================
         * BRANCH INSTRUCTIONS
         * ============================================================
         */

        char branch_condition[10];
        char label_name[50];

        /*
         * Try to parse:
         *
         * BEQ .loop
         * BGE .exit
         * BAL .loop
         */

        if (sscanf(line, "%s %s", branch_condition, label_name) == 2)
        {
            int branch_opcode = -1;

            if (strcmp(branch_condition, "BEQ") == 0)
                branch_opcode = 0x10;

            else if (strcmp(branch_condition, "BNE") == 0)
                branch_opcode = 0x11;

            else if (strcmp(branch_condition, "BCS") == 0)
                branch_opcode = 0x12;

            else if (strcmp(branch_condition, "BCC") == 0)
                branch_opcode = 0x13;

            else if (strcmp(branch_condition, "BMI") == 0)
                branch_opcode = 0x14;

            else if (strcmp(branch_condition, "BPL") == 0)
                branch_opcode = 0x15;

            else if (strcmp(branch_condition, "BVS") == 0)
                branch_opcode = 0x16;

            else if (strcmp(branch_condition, "BVC") == 0)
                branch_opcode = 0x17;

            else if (strcmp(branch_condition, "BHI") == 0)
                branch_opcode = 0x18;

            else if (strcmp(branch_condition, "BLS") == 0)
                branch_opcode = 0x19;

            else if (strcmp(branch_condition, "BGE") == 0)
                branch_opcode = 0x1A;

            else if (strcmp(branch_condition, "BLT") == 0)
                branch_opcode = 0x1B;

            else if (strcmp(branch_condition, "BGT") == 0)
                branch_opcode = 0x1C;

            else if (strcmp(branch_condition, "BLE") == 0)
                branch_opcode = 0x1D;

            else if (strcmp(branch_condition, "BAL") == 0)
                branch_opcode = 0x1E;

            if (branch_opcode != -1)
            {
                int target_address = find_label(label_name);

                if (target_address == -1)
                {
                    printf("Error: Label %s not found.\n",
                           label_name);

                    fclose(input);
                    fclose(output);

                    return;
                }

                /*
                 * PC after fetch already points to
                 * the next instruction.
                 *
                 * Therefore:
                 *
                 * relative offset =
                 * target - current_instruction_address
                 */

                int offset = target_address - (current_address + 4);

                printf("BRANCH: %s -> target=%d current=%d offset=%d\n",
                       label_name,
                       target_address,
                       current_address,
                       offset);

                /*
                 * Offset must fit into one signed byte.
                 */

                if (offset < -128 || offset > 127)
                {
                    printf("Error: Branch offset out of range.\n");

                    fclose(input);
                    fclose(output);

                    return;
                }

                fprintf(output,
                        "%02X 00 00 %02X\n",
                        branch_opcode,
                        (unsigned char)offset);

                current_address += 4;

                continue;
            }
        }

        /*
         * If no instruction matched,
         * report the line.
         */

        printf(
            "Warning: Could not compile line: %s\n",
            line);
    }

    /* =========================================================
       HALT

       00 00 00 00
       ========================================================= */

    fprintf(output,
            "00 00 00 00\n");

    fclose(input);
    fclose(output);

    printf("Compilation Successful.\n");
}
