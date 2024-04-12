#include <stdio.h>
#include "cpu.h"

int main(int argc, char **argv)
{
    unsigned char payload[] = {0x41, 0x42, 0x43, 0x44, 0x45};

    /*
    unsigned char code[] = {
        MOVI, R7, 0,
        LD, R0, R7,
        XORI, R0, 0x20,
        STR, R7, R0,
        ADDI, R7, 0x1,
        CMP, R7, 0x5,
        JNE, 2,
        HLT
    };
    */

    unsigned long code[] = {
        MOVI, R0, 1,
        MOV, R1, R0,
        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = payload;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    print_cpu_registers(cpu);
    print_cpu_flags(cpu);

    free_cpu(cpu);

    printf("payload: %s\n", payload);

    assert(cpu->registers[0] == 0);

    return 0;
}
