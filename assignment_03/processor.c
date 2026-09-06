#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "processor.h"
#include "memory.h"

/*
 * ============================================================
 * PROCESSOR REGISTERS
 * ============================================================
 */

int32_t Register[256];

/* 32 vector registers, each containing eight 32-bit elements. */
int32_t VectorRegister[32][8];

int PC;

int opcode;
int dest;
int src1;
int src2;

/*
 * ============================================================
 * CONDITION FLAGS
 * ============================================================
 */

int Z;
int N;
int C;
int V;

/*
 * ============================================================
 * SIMULATION CONTROL
 * ============================================================
 */

int end_of_simulation = 0;

/*
 * ============================================================
 * UPDATE Z AND N FLAGS
 * ============================================================
 *
 * Z = 1 if result == 0
 *
 * N = 1 if MSB of result is 1
 *     For a 32-bit value this is bit 31.
 * ============================================================
 */

void update_ZN(int32_t result)
{
    if (result == 0)
    {
        Z = 1;
    }
    else
    {
        Z = 0;
    }

    /*
     * Cast to uint32_t so that shifting is well-defined.
     */

    if (((uint32_t)result & 0x80000000U) != 0)
    {
        N = 1;
    }
    else
    {
        N = 0;
    }
}

/*
 * ============================================================
 * UPDATE FLAGS FOR ADDITION
 * ============================================================
 *
 * C:
 *     unsigned result is strictly less than either input.
 *
 * V:
 *     both operands have same sign
 *     AND result has different sign.
 * ============================================================
 */

void update_add_flags(int32_t a,
                      int32_t b,
                      int32_t result)
{
    uint32_t ua;
    uint32_t ub;
    uint32_t ur;

    ua = (uint32_t)a;
    ub = (uint32_t)b;
    ur = (uint32_t)result;

    /*
     * Z and N
     */

    update_ZN(result);

    /*
     * Carry flag.
     *
     * Unsigned addition overflow occurs when:
     *
     * result < either operand
     */

    if (ur < ua || ur < ub)
    {
        C = 1;
    }
    else
    {
        C = 0;
    }

    /*
     * Overflow flag.
     *
     * If both operands have the same sign
     * but result has a different sign.
     */

    if (((a >= 0) && (b >= 0) && (result < 0)) ||
        ((a < 0) && (b < 0) && (result >= 0)))
    {
        V = 1;
    }
    else
    {
        V = 0;
    }
}

/*
 * ============================================================
 * UPDATE FLAGS FOR SUBTRACTION
 * ============================================================
 *
 * C:
 *     1 if operand 1 is bigger than operand 2.
 *
 * V:
 *     operands have different signs
 *     AND result sign matches second operand.
 * ============================================================
 */

void update_sub_flags(int32_t a,
                      int32_t b,
                      int32_t result)
{
    /*
     * Z and N
     */

    update_ZN(result);

    /*
     * Carry flag according to assignment specification.
     */

    if (a > b)
    {
        C = 1;
    }
    else
    {
        C = 0;
    }

    /*
     * Overflow.
     *
     * Operand signs are different and
     * result sign matches operand 2.
     */

    if (((a >= 0) && (b < 0) && (result < 0)) ||
        ((a < 0) && (b >= 0) && (result >= 0)))
    {
        V = 1;
    }
    else
    {
        V = 0;
    }
}

/*
 * ============================================================
 * RESET
 * ============================================================
 */

void reset()
{
    for (int i = 0; i < 256; i++)
    {
        Register[i] = 0;
    }

    for (int v = 0; v < 32; v++)
    {
        for (int i = 0; i < 8; i++)
        {
            VectorRegister[v][i] = 0;
        }
    }

    PC = 0;

    opcode = 0;
    dest = 0;
    src1 = 0;
    src2 = 0;

    Z = 0;
    N = 0;
    C = 0;
    V = 0;

    end_of_simulation = 0;

    printf("Processor Reset Complete.\n");
}





/*
 * ============================================================
 * FETCH
 * ============================================================
 *
 * Instruction memory is byte-addressable.
 *
 * Every instruction contains four bytes:
 *
 *     opcode
 *     dest
 *     operand1
 *     operand2
 *
 * Therefore PC increases by 4.
 * ============================================================
 */

void fetch()
{
    opcode = (unsigned char)Instruction[PC];

    dest = (unsigned char)Instruction[PC + 1];

    src1 = (unsigned char)Instruction[PC + 2];

    src2 = (unsigned char)Instruction[PC + 3];

    printf(
        "FETCH -> PC=%3d | opcode=%02X dest=%d src1=%d src2=%d\n",
        PC,
        opcode,
        dest,
        src1,
        src2);

    PC += 4;
}

/*
 * ============================================================
 * DECODE
 * ============================================================
 *
 * No separate decode work is required yet.
 * ============================================================
 */

void decode()
{
}

int branch_condition(int opcode)
{
    switch (opcode)
    {
    case 0x10: // BEQ : Branch if equal
        return Z == 1;

    case 0x11: // BNE : Branch if not equal
        return Z == 0;

    case 0x12: // BCS : Branch if carry set
        return C == 1;

    case 0x13: // BCC : Branch if carry clear
        return C == 0;

    case 0x14: // BMI : Branch if negative
        return N == 1;

    case 0x15: // BPL : Branch if positive
        return N == 0;

    case 0x16: // BVS : Branch if overflow set
        return V == 1;

    case 0x17: // BVC : Branch if overflow clear
        return V == 0;

    case 0x18: // BHI : Branch if higher
        return C == 1 && Z == 0;

    case 0x19: // BLS : Branch if lower or same
        return C == 0 || Z == 1;

    case 0x1A: // BGE : Branch if greater or equal
        return N == V;

    case 0x1B: // BLT : Branch if less than
        return N != V;

    case 0x1C: // BGT : Branch if greater than
        return Z == 0 && N == V;

    case 0x1D: // BLE : Branch if less than or equal
        return Z == 1 || N != V;

    case 0x1E: // BAL : Branch always
        return 1;

    default:
        return 0;
    }
}

/*
 * ============================================================
 * EXECUTE
 * ============================================================
 */

void execute()
{
    int32_t a;
    int32_t b;
    int32_t result;

    switch (opcode)
    {

        /*
         * ====================================================
         * ADD VARIABLE
         *
         * 01 dest src1 src2
         *
         * dest = src1 + src2
         * ====================================================
         */

    case 0x10: // BEQ
    case 0x11: // BNE
    case 0x12: // BCS
    case 0x13: // BCC
    case 0x14: // BMI
    case 0x15: // BPL
    case 0x16: // BVS
    case 0x17: // BVC
    case 0x18: // BHI
    case 0x19: // BLS
    case 0x1A: // BGE
    case 0x1B: // BLT
    case 0x1C: // BGT
    case 0x1D: // BLE
    case 0x1E: // BAL
    {
        signed char offset = (signed char)src2;

        int taken = branch_condition(opcode);

        printf("BRANCH: opcode=%02X offset=%d condition=%s\n",
               opcode,
               offset,
               taken ? "TAKEN" : "NOT TAKEN");

        if (taken)
        {
            PC = PC + offset;
        }

        break;
    }

    case 0x01:

        a = Register[src1];
        b = Register[src2];

        result = a + b;

        Register[dest] = result;

        update_add_flags(a, b, result);

        printf(
            "ADD   : x%d = x%d + x%d = %d\n",
            dest,
            src1,
            src2,
            Register[dest]);

        break;

        /*
         * ====================================================
         * SUBTRACT VARIABLE
         *
         * 02 dest src1 src2
         *
         * dest = src1 - src2
         * ====================================================
         */

    case 0x02:

        a = Register[src1];
        b = Register[src2];

        result = a - b;

        Register[dest] = result;

        update_sub_flags(a, b, result);

        printf(
            "SUB   : x%d = x%d - x%d = %d\n",
            dest,
            src1,
            src2,
            Register[dest]);

        break;

        /*
         * ====================================================
         * MULTIPLY VARIABLE
         *
         * 03 dest src1 src2
         *
         * dest = src1 * src2
         *
         * Multiplication does not update flags according to
         * the assignment specification.
         * ====================================================
         */

    case 0x03:

        a = Register[src1];
        b = Register[src2];

        result = a * b;

        Register[dest] = result;

        printf(
            "MUL   : x%d = x%d * x%d = %d\n",
            dest,
            src1,
            src2,
            Register[dest]);

        break;

        /*
         * ====================================================
         * DIVIDE VARIABLE
         *
         * 04 dest src1 src2
         *
         * dest = src1 / src2
         * ====================================================
         */

    case 0x04:

        a = Register[src1];
        b = Register[src2];

        if (b == 0)
        {
            printf("Error: Division by zero!\n");

            end_of_simulation = 1;

            break;
        }

        /*
         * Special case:
         *
         * INT32_MIN / -1 cannot be represented
         * as a signed 32-bit integer.
         */

        if (a == INT32_MIN && b == -1)
        {
            printf("Error: 32-bit division overflow!\n");

            end_of_simulation = 1;

            break;
        }

        result = a / b;

        Register[dest] = result;

        printf(
            "DIV   : x%d = x%d / x%d = %d\n",
            dest,
            src1,
            src2,
            Register[dest]);

        break;

        /*
         * ====================================================
         * ADD CONSTANT
         *
         * 09 dest src1 constant
         *
         * Example:
         *
         * x1 = x2 + 10
         *
         * 09 01 02 0A
         * ====================================================
         */

    case 0x09:

        a = Register[src1];

        b = (int32_t)(unsigned char)src2;

        result = a + b;

        Register[dest] = result;

        update_add_flags(a, b, result);

        printf(
            "ADD-C : x%d = x%d + %d = %d\n",
            dest,
            src1,
            b,
            Register[dest]);

        break;

        /*
         * ====================================================
         * SUBTRACT CONSTANT
         *
         * 0A dest src1 constant
         *
         * x1 = x2 - 10
         * ====================================================
         */

    case 0x0A:

        a = Register[src1];

        b = (int32_t)(unsigned char)src2;

        result = a - b;

        Register[dest] = result;

        update_sub_flags(a, b, result);

        printf(
            "SUB-C : x%d = x%d - %d = %d\n",
            dest,
            src1,
            b,
            Register[dest]);

        break;

        /*
         * ====================================================
         * MULTIPLY CONSTANT
         *
         * 0B dest src1 constant
         *
         * x1 = x2 * 10
         * ====================================================
         */

    case 0x0B:

        a = Register[src1];

        b = (int32_t)(unsigned char)src2;

        result = a * b;

        Register[dest] = result;

        printf(
            "MUL-C : x%d = x%d * %d = %d\n",
            dest,
            src1,
            b,
            Register[dest]);

        break;

        /*
         * ====================================================
         * DIVIDE CONSTANT
         *
         * 0C dest src1 constant
         *
         * x1 = x2 / 10
         * ====================================================
         */

    case 0x0C:

        a = Register[src1];

        b = (int32_t)(unsigned char)src2;

        if (b == 0)
        {
            printf("Error: Division by zero!\n");

            end_of_simulation = 1;

            break;
        }

        result = a / b;

        Register[dest] = result;

        printf(
            "DIV-C : x%d = x%d / %d = %d\n",
            dest,
            src1,
            b,
            Register[dest]);

        break;

        /* ============================================================
         * MEMORY READ - VARIABLE ADDRESS
         *
         * x[dest] = [x[src1]]
         * ============================================================
         */

    case 0x05:
    {
        int address = Register[src1];

        Register[dest] = read_memory_32(address);

        printf(
            "LOAD-V : x%d = [x%d] -> Data[%d] = %d\n",
            dest,
            src1,
            address,
            Register[dest]);

        break;
    }

        /* ============================================================
         * MEMORY WRITE - VARIABLE ADDRESS
         *
         * [x[src2]] = x[src1]
         * ============================================================
         */

    case 0x06:
    {
        int address = Register[src2];

        write_memory_32(
            address,
            Register[src1]);

        printf(
            "STORE-V: [x%d] -> Data[%d] = x%d = %d\n",
            src2,
            address,
            src1,
            Register[src1]);

        break;
    }

        /* ============================================================
         * MEMORY READ - CONSTANT ADDRESS
         *
         * x[dest] = [src2]
         * ============================================================
         */

    case 0x0D:
    {
        int address = src2;

        Register[dest] = read_memory_32(address);

        printf(
            "LOAD-C : x%d = Data[%d] = %d\n",
            dest,
            address,
            Register[dest]);

        break;
    }

        /* ============================================================
         * MEMORY WRITE - CONSTANT ADDRESS
         *
         * [src2] = x[src1]
         * ============================================================
         */

    case 0x0E:
    {
        int address = src2;

        write_memory_32(
            address,
            Register[src1]);

        printf(
            "STORE-C: Data[%d] = x%d = %d\n",
            address,
            src1,
            Register[src1]);

        break;
    }

        /*
         * ====================================================
         * DATA MOVEMENT - CONSTANT
         *
         * 0F dest 0 constant
         *
         * Example:
         *
         * x1 = 100
         *
         * 0F 01 00 64
         * ====================================================
         */

    case 0x0F:

        Register[dest] =
            (int32_t)(unsigned char)src2;

        printf(
            "MOVE  : x%d = %d\n",
            dest,
            Register[dest]);

        break;


        /* ============================================================
         * VECTOR ARITHMETIC
         *
         * Vector registers contain eight 32-bit elements.
         * Vector operations do not modify the scalar flags.
         * ============================================================
         */
    case 0x21:
    case 0x22:
    case 0x23:
    {
        for (int i = 0; i < 8; i++)
        {
            if (opcode == 0x21)
                VectorRegister[dest][i] = VectorRegister[src1][i] + VectorRegister[src2][i];
            else if (opcode == 0x22)
                VectorRegister[dest][i] = VectorRegister[src1][i] - VectorRegister[src2][i];
            else
                VectorRegister[dest][i] = VectorRegister[src1][i] * VectorRegister[src2][i];
        }
        printf("VECTOR OP: v%d = v%d %s v%d\n",
               dest, src1, opcode == 0x21 ? "+" : opcode == 0x22 ? "-" : "*", src2);
        break;
    }

    case 0x29:
    case 0x2A:
    case 0x2B:
    {
        int32_t value = (int32_t)(unsigned char)src2;
        for (int i = 0; i < 8; i++)
        {
            if (opcode == 0x29)
                VectorRegister[dest][i] = VectorRegister[src1][i] + value;
            else if (opcode == 0x2A)
                VectorRegister[dest][i] = VectorRegister[src1][i] - value;
            else
                VectorRegister[dest][i] = VectorRegister[src1][i] * value;
        }
        printf("VECTOR CONST OP: v%d updated using %d\n", dest, value);
        break;
    }

    case 0x25:
    {
        int address = Register[src1];
        for (int i = 0; i < 8; i++)
            VectorRegister[dest][i] = read_memory_32(address + 4 * i);
        printf("VECTOR LOAD: v%d <- Data[%d..%d]\n",
               dest, address, address + 28);
        break;
    }

    case 0x2C:
    {
        int address = src2;
        for (int i = 0; i < 8; i++)
            VectorRegister[dest][i] = read_memory_32(address + 4 * i);
        printf("VECTOR LOAD CONST: v%d <- Data[%d..%d]\n",
               dest, address, address + 28);
        break;
    }

    case 0x26:
    {
        int address = Register[src2];
        for (int i = 0; i < 8; i++)
            write_memory_32(address + 4 * i, VectorRegister[src1][i]);
        printf("VECTOR STORE: v%d -> Data[%d..%d]\n",
               src1, address, address + 28);
        break;
    }

    case 0x2E:
    {
        int address = src2;
        for (int i = 0; i < 8; i++)
            write_memory_32(address + 4 * i, VectorRegister[src1][i]);
        printf("VECTOR STORE CONST: v%d -> Data[%d..%d]\n",
               src1, address, address + 28);
        break;
    }

        /*
         * ====================================================
         * HALT
         *
         * 00 00 00 00
         * ====================================================
         */

    case 0x00:

        printf("HALT\n");

        end_of_simulation = 1;

        break;

        /*
         * ====================================================
         * UNKNOWN OPCODE
         * ====================================================
         */

    default:

        printf(
            "Unknown Opcode: 0x%02X\n",
            opcode);

        end_of_simulation = 1;

        break;
    }

    /*
     * Print flags after every instruction.
     *
     * This is extremely useful while debugging Lab 2.
     */

    printf(
        "FLAGS -> Z=%d N=%d C=%d V=%d\n\n",
        Z,
        N,
        C,
        V);
}
