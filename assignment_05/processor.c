#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include "processor.h"

int32_t Register[NP][256];
int32_t VectorRegister[NP][32][8];
int PC[NP];
int opcode, dest, src1, src2;
int Z[NP], N[NP], C[NP], V[NP];
int process_pid[NP];
int end_of_simulation[NP];
int proc_id = 0;
FILE *fd_log = NULL;

static void update_ZN(int id, int32_t r) {
    Z[id] = (r == 0);
    N[id] = (((uint32_t)r & 0x80000000U) != 0);
}
static void update_add_flags(int id,int32_t a,int32_t b,int32_t r) {
    uint32_t ua=(uint32_t)a, ub=(uint32_t)b, ur=(uint32_t)r;
    update_ZN(id,r);
    C[id]=(ur<ua || ur<ub);
    V[id]=((a>=0 && b>=0 && r<0) || (a<0 && b<0 && r>=0));
}
static void update_sub_flags(int id,int32_t a,int32_t b,int32_t r) {
    update_ZN(id,r);
    C[id]=(a>b);
    V[id]=((a>=0 && b<0 && r<0) || (a<0 && b>=0 && r>=0));
}
static int branch_condition(int id, int op) {
    switch(op) {
        case 0x10:return Z[id]==1; case 0x11:return Z[id]==0; case 0x12:return C[id]==1;
        case 0x13:return C[id]==0; case 0x14:return N[id]==1; case 0x15:return N[id]==0;
        case 0x16:return V[id]==1; case 0x17:return V[id]==0; case 0x18:return C[id]==1&&Z[id]==0;
        case 0x19:return C[id]==0||Z[id]==1; case 0x1A:return N[id]==V[id]; case 0x1B:return N[id]!=V[id];
        case 0x1C:return Z[id]==0&&N[id]==V[id]; case 0x1D:return Z[id]==1||N[id]!=V[id]; case 0x1E:return 1;
        default:return 0;
    }
}
void reset(int id) {
    int i,j;
    if(id<0||id>=NP) return;
    for(i=0;i<256;i++) Register[id][i]=0;
    for(i=0;i<32;i++) for(j=0;j<8;j++) VectorRegister[id][i][j]=0;
    PC[id]=0; end_of_simulation[id]=0;
    Z[id]=N[id]=C[id]=V[id]=0; process_pid[id]=0;
}
void fetch(int id)
{
    int p0, p1, p2, p3;

    proc_id = id;

    if (PC[id] < 0 || PC[id] + 3 >= INSTRUCTION_LOGICAL_SIZE) {
        end_of_simulation[id] = 1;
        return;
    }

    /* Lab 5: translate every instruction-byte access through the MMU. */
    p0 = getPhysicallAddress(id, 1, PC[id]);
    p1 = getPhysicallAddress(id, 1, PC[id] + 1);
    p2 = getPhysicallAddress(id, 1, PC[id] + 2);
    p3 = getPhysicallAddress(id, 1, PC[id] + 3);

    if (p0 < 0 || p1 < 0 || p2 < 0 || p3 < 0) {
        end_of_simulation[id] = 1;
        return;
    }

    opcode = memory[p0];
    dest   = memory[p1];
    src1   = memory[p2];
    src2   = memory[p3];
    PC[id] += 4;
}

void decode(void) {}

static void log_print(int id, int reg) {
    if(fd_log) {
        fprintf(fd_log, "[PRINT] Process id: %d : x%d : %08X\n",
                process_pid[id], reg, (uint32_t)Register[id][reg]);
        fflush(fd_log);
    }
}
void set_processor_pid(int id, int pid) { if (id >= 0 && id < NP) process_pid[id] = pid; }

void execute(int id) {
    int32_t a,b,r; int i,address; int taken;
    proc_id=id;
    switch(opcode) {
        case 0x00: end_of_simulation[id]=1; break;
        case 0x01: a=Register[id][src1]; b=Register[id][src2]; r=a+b; Register[id][dest]=r; update_add_flags(id,a,b,r); break;
        case 0x02: a=Register[id][src1]; b=Register[id][src2]; r=a-b; Register[id][dest]=r; update_sub_flags(id,a,b,r); break;
        case 0x03: Register[id][dest]=Register[id][src1]*Register[id][src2]; break;
        case 0x04: a=Register[id][src1]; b=Register[id][src2]; if(b==0 || (a==INT32_MIN&&b==-1)){fprintf(stderr,"Process %d: invalid division\n",id);end_of_simulation[id]=1;} else Register[id][dest]=a/b; break;
        case 0x05: address=Register[id][src1]; Register[id][dest]=read_memory_32(id,address); break;
        case 0x06: address=Register[id][src2]; write_memory_32(id,address,Register[id][src1]); break;
        case 0x07: Register[id][dest]=Register[id][src1]; break;
        case 0x08: log_print(id,src2); break;
        case 0x09: a=Register[id][src1]; b=(unsigned char)src2; r=a+b; Register[id][dest]=r; update_add_flags(id,a,b,r); break;
        case 0x0A: a=Register[id][src1]; b=(unsigned char)src2; r=a-b; Register[id][dest]=r; update_sub_flags(id,a,b,r); break;
        case 0x0B: Register[id][dest]=Register[id][src1]*(unsigned char)src2; break;
        case 0x0C: a=Register[id][src1]; b=(unsigned char)src2; if(!b) end_of_simulation[id]=1; else Register[id][dest]=a/b; break;
        case 0x0D: Register[id][dest]=read_memory_32(id,src2); break;
        case 0x0E: write_memory_32(id,src2,Register[id][src1]); break;
        case 0x0F: Register[id][dest]=(unsigned char)src2; break;
        case 0x10:case 0x11:case 0x12:case 0x13:case 0x14:case 0x15:case 0x16:
        case 0x17:case 0x18:case 0x19:case 0x1A:case 0x1B:case 0x1C:case 0x1D:case 0x1E:
            taken=branch_condition(id,opcode); if(taken) PC[id]+=(int8_t)src2; break;
        case 0x21:case 0x22:case 0x23:
            for(i=0;i<8;i++) {
                if(opcode==0x21) VectorRegister[id][dest][i]=VectorRegister[id][src1][i]+VectorRegister[id][src2][i];
                else if(opcode==0x22) VectorRegister[id][dest][i]=VectorRegister[id][src1][i]-VectorRegister[id][src2][i];
                else VectorRegister[id][dest][i]=VectorRegister[id][src1][i]*VectorRegister[id][src2][i];
            } break;
        case 0x29:case 0x2A:case 0x2B:
            b=(unsigned char)src2;
            for(i=0;i<8;i++) {
                if(opcode==0x29) VectorRegister[id][dest][i]=VectorRegister[id][src1][i]+b;
                else if(opcode==0x2A) VectorRegister[id][dest][i]=VectorRegister[id][src1][i]-b;
                else VectorRegister[id][dest][i]=VectorRegister[id][src1][i]*b;
            } break;
        case 0x25: address=Register[id][src1]; for(i=0;i<8;i++) VectorRegister[id][dest][i]=read_memory_32(id,address+4*i); break;
        case 0x2C: address=src2; for(i=0;i<8;i++) VectorRegister[id][dest][i]=read_memory_32(id,address+4*i); break;
        case 0x26: address=Register[id][src2]; for(i=0;i<8;i++) write_memory_32(id,address+4*i,VectorRegister[id][src1][i]); break;
        case 0x2E: address=src2; for(i=0;i<8;i++) write_memory_32(id,address+4*i,VectorRegister[id][src1][i]); break;
        default: fprintf(stderr,"Process %d: unknown opcode 0x%02X\n",id,opcode); end_of_simulation[id]=1; break;
    }
}
void process_instructions(int id,int instruction_count) {
    int i;
    for(i=0;i<instruction_count && !end_of_simulation[id];i++){ fetch(id); if(!end_of_simulation[id]){decode();execute(id);} }
    {
        struct timespec ts={0,10000}; /* 10 microseconds */
        nanosleep(&ts,NULL);
    }
}
