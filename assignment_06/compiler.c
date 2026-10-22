#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compiler.h"

#define MAX_LABELS 100
#define MAX_LINE 512
#define MAX_INSTRUCTIONS 256

typedef struct {
    char name[64];
    int address;
} Label;

static Label labels[MAX_LABELS];
static int label_count;

static int find_label(const char *name)
{
    int i;
    for (i = 0; i < label_count; ++i)
        if (strcmp(labels[i].name, name) == 0)
            return labels[i].address;
    return -1;
}

static void strip_newline(char *s)
{
    s[strcspn(s, "\r\n")] = '\0';
}

static int clean_line(char *line)
{
    char *comment = strchr(line, '%');
    if (comment)
        *comment = '\0';
    return line[0] != '\0' && line[0] != '\n' && line[0] != '\r';
}

static int valid_reg(int r)
{
    return r >= 0 && r < 256;
}

static int valid_vreg(int r)
{
    return r >= 0 && r < 32;
}

static int valid_byte(int v)
{
    return v >= 0 && v <= 255;
}

static int write_instruction(FILE *output, FILE *map, int address, int opcode,
                             int dest, int src1, int src2, int source_line,
                             const char *source)
{
    if (address < 0 || address / 4 >= MAX_INSTRUCTIONS)
        return 0;
    fprintf(output, "%02X %02X %02X %02X\n", opcode & 255, dest & 255,
            src1 & 255, src2 & 255);
    fprintf(map, "%d\t%d\t%s\n", address / 4, source_line, source);
    return 1;
}

static int first_pass(const char *source_file)
{
    FILE *fp = fopen(source_file, "r");
    char line[MAX_LINE];
    int address = 0;

    if (!fp)
        return 0;

    label_count = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (!clean_line(line))
            continue;
        strip_newline(line);
        if (line[0] == '.') {
            if (label_count >= MAX_LABELS) {
                fclose(fp);
                return 0;
            }
            if (strlen(line) >= sizeof(labels[0].name)) {
                fclose(fp);
                return 0;
            }
            strcpy(labels[label_count].name, line);
            labels[label_count].address = address;
            ++label_count;
        } else {
            address += 4;
            if (address > MAX_INSTRUCTIONS * 4) {
                fclose(fp);
                return 0;
            }
        }
    }
    fclose(fp);
    return 1;
}

void compile(const char *source_file, const char *program_file)
{
    FILE *input;
    FILE *output;
    FILE *map;
    char map_file[MAX_LINE];
    char raw[MAX_LINE];
    int current_address = 0;
    int source_line = 0;

    if (!source_file || !program_file)
        return;

    if (!first_pass(source_file)) {
        fprintf(stderr, "Error: Cannot prepare %s\n", source_file);
        return;
    }

    input = fopen(source_file, "r");
    output = fopen(program_file, "w");
    if (snprintf(map_file, sizeof(map_file), "%s.map", program_file) >= (int)sizeof(map_file)) {
        if (input) fclose(input);
        if (output) fclose(output);
        return;
    }
    map = fopen(map_file, "w");

    if (!input || !output || !map) {
        if (input) fclose(input);
        if (output) fclose(output);
        if (map) fclose(map);
        return;
    }

    while (fgets(raw, sizeof(raw), input)) {
        char source[MAX_LINE];
        char line[MAX_LINE];
        int dest, src1, src2, constant;
        char operation;

        ++source_line;
        snprintf(source, sizeof(source), "%s", raw);
        strip_newline(source);
        snprintf(line, sizeof(line), "%s", raw);
        if (!clean_line(line))
            continue;
        strip_newline(line);
        if (line[0] == '.')
            continue;

        if (sscanf(line, "Print x%d", &src2) == 1) {
            if (!valid_reg(src2) || !write_instruction(output, map, current_address, 0x08, 0, 0, src2, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "Read x%d, %d", &dest, &constant) == 2) {
            if (!valid_reg(dest) || !valid_byte(constant) || !write_instruction(output, map, current_address, 0x0D, dest, 0, constant, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "Write x%d, %d", &dest, &constant) == 2) {
            if (!valid_reg(dest) || !valid_byte(constant) || !write_instruction(output, map, current_address, 0x0E, 0, dest, constant, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "x%d = [x%d]", &dest, &src1) == 2) {
            if (!valid_reg(dest) || !valid_reg(src1) || !write_instruction(output, map, current_address, 0x05, dest, src1, 0, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "x%d = [%d]", &dest, &constant) == 2) {
            if (!valid_reg(dest) || !valid_byte(constant) || !write_instruction(output, map, current_address, 0x0D, dest, 0, constant, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "[x%d] = x%d", &dest, &src1) == 2) {
            if (!valid_reg(dest) || !valid_reg(src1) || !write_instruction(output, map, current_address, 0x06, 0, src1, dest, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "[%d] = x%d", &constant, &dest) == 2) {
            if (!valid_byte(constant) || !valid_reg(dest) || !write_instruction(output, map, current_address, 0x0E, 0, dest, constant, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "v%d = [x%d]", &dest, &src1) == 2) {
            if (!valid_vreg(dest) || !valid_reg(src1) || !write_instruction(output, map, current_address, 0x25, dest, src1, 0, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "v%d = [%d]", &dest, &constant) == 2) {
            if (!valid_vreg(dest) || !valid_byte(constant) || !write_instruction(output, map, current_address, 0x2C, dest, 0, constant, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "[x%d] = v%d", &src1, &dest) == 2) {
            if (!valid_reg(src1) || !valid_vreg(dest) || !write_instruction(output, map, current_address, 0x26, 0, dest, src1, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "[%d] = v%d", &constant, &dest) == 2) {
            if (!valid_byte(constant) || !valid_vreg(dest) || !write_instruction(output, map, current_address, 0x2E, 0, dest, constant, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "v%d = v%d %c v%d", &dest, &src1, &operation, &src2) == 4) {
            int op = operation == '+' ? 0x21 : operation == '-' ? 0x22 : operation == '*' ? 0x23 : -1;
            if (op < 0 || !valid_vreg(dest) || !valid_vreg(src1) || !valid_vreg(src2) || !write_instruction(output, map, current_address, op, dest, src1, src2, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "v%d = v%d %c %d", &dest, &src1, &operation, &constant) == 4) {
            int op = operation == '+' ? 0x29 : operation == '-' ? 0x2A : operation == '*' ? 0x2B : -1;
            if (op < 0 || !valid_vreg(dest) || !valid_vreg(src1) || !valid_byte(constant) || !write_instruction(output, map, current_address, op, dest, src1, constant, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "x%d = %d", &dest, &constant) == 2) {
            if (!valid_reg(dest) || !valid_byte(constant) || !write_instruction(output, map, current_address, 0x0F, dest, 0, constant, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "x%d = x%d %c x%d", &dest, &src1, &operation, &src2) == 4) {
            int op = operation == '+' ? 0x01 : operation == '-' ? 0x02 : operation == '*' ? 0x03 : operation == '/' ? 0x04 : -1;
            if (op < 0 || !valid_reg(dest) || !valid_reg(src1) || !valid_reg(src2) || !write_instruction(output, map, current_address, op, dest, src1, src2, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        if (sscanf(line, "x%d = x%d %c %d", &dest, &src1, &operation, &constant) == 4) {
            int op = operation == '+' ? 0x09 : operation == '-' ? 0x0A : operation == '*' ? 0x0B : operation == '/' ? 0x0C : -1;
            if (op < 0 || !valid_reg(dest) || !valid_reg(src1) || !valid_byte(constant) || !write_instruction(output, map, current_address, op, dest, src1, constant, source_line, source))
                goto fail;
            current_address += 4;
            continue;
        }

        {
            char branch[16];
            char label[64];
            if (sscanf(line, "%15s %63s", branch, label) == 2) {
                int op = -1;
                int target;
                int offset;
                if (strcmp(branch, "BEQ") == 0) op = 0x10;
                else if (strcmp(branch, "BNE") == 0) op = 0x11;
                else if (strcmp(branch, "BCS") == 0) op = 0x12;
                else if (strcmp(branch, "BCC") == 0) op = 0x13;
                else if (strcmp(branch, "BMI") == 0) op = 0x14;
                else if (strcmp(branch, "BPL") == 0) op = 0x15;
                else if (strcmp(branch, "BVS") == 0) op = 0x16;
                else if (strcmp(branch, "BVC") == 0) op = 0x17;
                else if (strcmp(branch, "BHI") == 0) op = 0x18;
                else if (strcmp(branch, "BLS") == 0) op = 0x19;
                else if (strcmp(branch, "BGE") == 0) op = 0x1A;
                else if (strcmp(branch, "BLT") == 0) op = 0x1B;
                else if (strcmp(branch, "BGT") == 0) op = 0x1C;
                else if (strcmp(branch, "BLE") == 0) op = 0x1D;
                else if (strcmp(branch, "BAL") == 0) op = 0x1E;
                if (op >= 0) {
                    target = find_label(label);
                    if (target < 0) goto fail;
                    offset = target - (current_address + 4);
                    if (offset < -128 || offset > 127) goto fail;
                    if (!write_instruction(output, map, current_address, op, 0, 0, (unsigned char)offset, source_line, source))
                        goto fail;
                    current_address += 4;
                    continue;
                }
            }
        }

        fprintf(stderr, "Warning: Could not compile line %d: %s\n", source_line, source);
    }

    fprintf(output, "00 00 00 00\n");
    fclose(input);
    fclose(output);
    fclose(map);
    return;

fail:
    fclose(input);
    fclose(output);
    fclose(map);
    remove(program_file);
    remove(map_file);
}
