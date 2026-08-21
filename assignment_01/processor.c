#include <stdio.h>
#include "processor.h"
#include "memory.h"

int Register[256];
int PC;

int opcode, dest, src1, src2;
int end_of_simulation = 0;

void reset()
{
    for (int i = 0; i < 256; i++)
    {
        Register[i] = 0;
    }

    PC = 0;
    opcode = dest = src1 = src2 = 0;
    end_of_simulation = 0;

    printf("Processor reset successfully.\n\n");
}

void fetch()
{
    opcode = (unsigned char)Instruction[PC];
    dest = (unsigned char)Instruction[PC + 1];
    src1 = (unsigned char)Instruction[PC + 2];
    src2 = (unsigned char)Instruction[PC + 3];

    printf("Fetch -> PC = %d | opcode = %d | dest = %d | src1 = %d | src2 = %d\n", PC, opcode, dest, src1, src2);
    PC += 4;
}

void decode()
{
}

void execute()
{
    switch (opcode)
    {
    case 1: // Add
        Register[dest] = Register[src1] + Register[src2];
        printf("Add -> x%d = x%d + x%d | Result = %d\n\n", dest, src1, src2, Register[dest]);
        break;

    case 2: // Subtract
        Register[dest] = Register[src1] - Register[src2];
        printf("Subtract -> x%d = x%d - x%d | Result = %d\n\n", dest, src1, src2, Register[dest]);
        break;

    case 3: // Multiply
        Register[dest] = Register[src1] * Register[src2];
        printf("Multiply -> x%d = x%d * x%d | Result = %d\n\n", dest, src1, src2, Register[dest]);
        break;

    case 4: // Divide
        if (Register[src2] == 0)
        {
            printf("Error: Division by zero. Halting execution.\n");
            end_of_simulation = 1;
            return;
        }
        Register[dest] = Register[src1] / Register[src2];
        printf("Divide -> x%d = x%d / x%d | Result = %d\n\n", dest, src1, src2, Register[dest]);
        break;

    case 5: // Read
        Register[dest] = (unsigned char)Data[src1];
        printf("Read -> x%d = Data[%d] | Result = %d\n\n", dest, src1, Register[dest]);
        break;

    case 6: // Write
        Data[src1] = (char)Register[dest];
        printf("Write -> Data[%d] = x%d | Result = %d\n\n", src1, dest, Data[src1]);
        break;

    case 7: // Move
        Register[dest] = src1;
        printf("Move -> x%d = %d | Result = %d\n", dest, src1, Register[dest]);
        break;

    case 0: // Halt
        printf("Halt -> Halting execution.\n");
        end_of_simulation = 1;
        break;

    default:
        printf("Error: Invalid opcode %d. Halting execution.\n", opcode);
        end_of_simulation = 1;
        break;
    }
}
