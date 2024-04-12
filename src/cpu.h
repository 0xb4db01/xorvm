#pragma once

/**
 * XORVM CPU.h implementation.
 * Author: 0xb4db01
 */

#include <stdlib.h>

#include "instructions.h"

typedef struct memory_t
{
    size_t code_size;
    size_t rwmem_size;
    double *code;
    unsigned char *rwmem;
} Memory;

/*
 * R<n>: common registers (long)
 * D<n>: bigint/float registers (double)
 */
enum Registers
{
    R0, R1, R2, R3, R4, R5, R6, R7,
    D0, D1, D2, D3, D4, D5, D6, D7,
    TOTAL_REGISTERS
};

/*
 * Instructions are defined in instructions.h...
 */
typedef struct instruction_t
{
    int bytecode;
} Instruction;

typedef struct flags_t
{
    short zero;
    short negative;
    short overflow;
} Flags;


typedef struct cpu_t
{
    Memory *memory;

    long PC;
    
    long registers[8];
    double dregisters[8];

    Instruction instruction;

    Flags flags;
} CPU;

/*
 * CPU Functions prototypes
 */

CPU *new_cpu(Memory *);
void free_cpu(CPU *);
void fetch_instruction(CPU *);
void set_flags(CPU *, int);
void execute_instruction(CPU *);
void run_cpu(CPU *);

/*
 * Instruction functions prototypes
 */

void move(CPU *);
void add(CPU *);
void addi(CPU *);
void sub(CPU *);
void subi(CPU *);
void mul(CPU *);
void muli(CPU *);
void _div(CPU *);
void divi(CPU *);
void movei(CPU *);
void cmp(CPU *);
void jmp(CPU *);
void je(CPU *);
void jne(CPU *);
void jlt(CPU *);
void jgt(CPU *);
void load(CPU *);
void store(CPU *);
void _xor(CPU *);
void xori(CPU *);
void shl(CPU *);
void shli(CPU *);
void shr(CPU *);
void shri(CPU *);

/*
 * CPU utility functions prototypes
 */
void print_cpu_registers(CPU *);
void print_cpu_flags(CPU *);

/*
 * CPU Functions implementation
 */

CPU *new_cpu(Memory *memory)
{
    CPU *cpu = (CPU *)malloc(sizeof(CPU));
    
    cpu->memory = memory;

    cpu->PC = -1;

    cpu->instruction.bytecode = 0;

    return cpu;
}

void free_cpu(CPU *cpu)
{
    for(size_t i = 0; i <= 7; ++i)
    {
        cpu->registers[i] = 0;
        cpu->dregisters[i] = 0;
    }

    free(cpu);
}

void fetch_instruction(CPU *cpu)
{
    cpu->PC++;
    cpu->instruction.bytecode = cpu->memory->code[cpu->PC];
}

void set_flags(CPU *cpu, int dst)
{
    cpu->flags.negative = (cpu->registers[dst] < 0) ? 1: 0;
    cpu->flags.zero = (cpu->registers[dst] == 0) ? 1: 0;
}

/*
 * For each instruction defined in cpu->memory->code, the appropriate 
 * implementation function is called.
 * If the instruction is not listed in the Instruction enum, the program
 * prints an error on stdout.
 */
void execute_instruction(CPU *cpu)
{
    switch(cpu->instruction.bytecode)
    {
        case MOV:
            move(cpu);

            break;

        case MOVI:
            movei(cpu);

            break;

        case ADD:
            add(cpu);

            break;

        case ADDI:
            addi(cpu);

            break;

        case SUB:
            sub(cpu);

            break;

        case SUBI:
            subi(cpu);

            break;

        case MUL:
            mul(cpu);

            break;

        case MULI:
            muli(cpu);

            break;

        case DIV:
            _div(cpu);

            break;

        case DIVI:
            divi(cpu);

            break;

        case CMP:
            cmp(cpu);

            break;

        case JMP:
            jmp(cpu);

            break;

        case JE:
            je(cpu);

            break;

        case JNE:
            jne(cpu);

            break;

        case JLT:
            jlt(cpu);

            break;

        case JGT:
            jgt(cpu);

            break;

        case LD:
            load(cpu);

            break;

        case STR:
            store(cpu);

            break;

        case XOR:
            _xor(cpu);

            break;

        case XORI:
            xori(cpu);

            break;

        case SHL:
            shl(cpu);

            break;

        case SHLI:
            shli(cpu);

            break;

        case SHR:
            shr(cpu);

            break;

        case SHRI:
            shri(cpu);

            break;

        case HLT:
            break;

        default:
            printf("Unsupported instruction... %d\n",
                    cpu->instruction.bytecode);

            break;
    }
}

/*
 * This is the main function to be called when a new CPU is created.
 * It fetches the instruction in cpu->memory->code and executes it.
 */
void run_cpu(CPU *cpu)
{
    while(cpu->instruction.bytecode != HLT)
    {
        fetch_instruction(cpu);
        execute_instruction(cpu);
    }
}

void move(CPU *cpu)
{
    double dst = cpu->memory->code[cpu->PC+1];
    double src = cpu->memory->code[cpu->PC+2];

    if(dst <=7 && src <= 7)
    {
        cpu->registers[(int)dst] = cpu->registers[(int)src];
    } else if(dst <= 7 && src >= 7)
    {
        cpu->registers[(int)dst] = cpu->dregisters[(int)src - 8];
    } else if(dst >=7 && src <= 7)
    {
        cpu->dregisters[(int)dst - 8] = cpu->registers[(int)src];
    } else if(dst >= 7 && src >= 7)
    {
        cpu->dregisters[(int)dst - 8] = cpu->dregisters[(int)src - 8];
    }

    set_flags(cpu, dst);

    cpu->PC += 2;
}

void movei(CPU *cpu)
{
    double dst = cpu->memory->code[cpu->PC+1];
    double src = cpu->memory->code[cpu->PC+2];

    if(dst <=7)
    {
        cpu->registers[(int)dst] = src;
    } else
    {
        cpu->dregisters[(int)dst - 8] = src;
    }

    set_flags(cpu, dst);

    cpu->PC += 2;
}

/*
 * Operation instructions.
 *
 * The following macros are just for convenience so there is less code
 * repetition.
 */

#define OPERATION(operator)\
    double dst = cpu->memory->code[cpu->PC+1];\
    double src = cpu->memory->code[cpu->PC+2];\
    if(dst <=7 && src <= 7)\
    {\
        cpu->registers[(int)dst] operator##= cpu->registers[(int)src];\
    } else if(dst <= 7 && src >= 7)\
    {\
        cpu->registers[(int)dst] operator##= cpu->dregisters[(int)src - 8];\
    } else if(dst >=7 && src <= 7)\
    {\
        cpu->dregisters[(int)dst - 8] operator##= cpu->registers[(int)src];\
    } else if(dst >= 7 && src >= 7)\
    {\
        cpu->dregisters[(int)dst - 8] operator##=\
                cpu->dregisters[(int)src - 8];\
    }\

#define OPERATIONI(operator)\
    double dst = cpu->memory->code[cpu->PC+1];\
    double src = cpu->memory->code[cpu->PC+2];\
    if(dst <=7)\
    {\
        cpu->registers[(int)dst] operator##= src;\
    } else\
    {\
        cpu->dregisters[(int)dst - 8] operator##= src;\
    }\

void add(CPU *cpu)
{
    OPERATION(+)

    set_flags(cpu, dst);

    cpu->PC += 2;
}

void addi(CPU *cpu)
{

    OPERATIONI(+)

    set_flags(cpu, dst);

    cpu->PC += 2;
}

void sub(CPU *cpu)
{
    OPERATION(-)

    set_flags(cpu, dst);

    cpu->PC += 2;
}

void subi(CPU *cpu)
{
    OPERATIONI(-)

    set_flags(cpu, dst);

    cpu->PC += 2;
}

void mul(CPU *cpu)
{
    OPERATION(*);

    set_flags(cpu, dst);

    cpu->PC += 2;
}

void muli(CPU *cpu)
{
    OPERATIONI(*);

    set_flags(cpu, dst);

    cpu->PC += 2;
}

void _div(CPU *cpu)
{
    OPERATION(/)

    set_flags(cpu, dst);

    cpu->PC += 2;
}

void divi(CPU *cpu)
{
    OPERATIONI(/)

    set_flags(cpu, dst);

    cpu->PC += 2;
}

/*
 * Compare function:
 * DST == SRC --> flag.zero = 1
 * DST < SRC  --> flag.zero = 0, flag.overflow = 0
 * DST > SRC  --> flag.zero = 0, flag.overflow = 1
 */
void cmp(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];
    long src = cpu->memory->code[cpu->PC+2];

    cpu->flags.zero = 0;
    cpu->flags.negative = 0;
    cpu->flags.overflow = 0;
    
    if(dst <= 7 && src <= 7)
    {
        if(cpu->registers[dst] == cpu->registers[src])
            cpu->flags.zero = 1;
        else if(cpu->registers[dst] < cpu->registers[src])
        {
            cpu->flags.zero = 0;
            cpu->flags.overflow = 0;
        }   
        else if(cpu->registers[dst] > cpu->registers[src])
        {
            cpu->flags.zero = 0;
            cpu->flags.overflow = 1;
        }

    } else if(dst <= 7 && src > 7)
    {
        if(cpu->registers[dst] == cpu->dregisters[src - 8])
            cpu->flags.zero = 1;
        else if(cpu->registers[dst] < cpu->dregisters[src - 8])
        {
            cpu->flags.zero = 0;
            cpu->flags.overflow = 0;
        }   
        else if(cpu->registers[dst] > cpu->dregisters[src - 8])
        {
            cpu->flags.zero = 0;
            cpu->flags.overflow = 1;
        }
    } else if(dst > 7 && src <= 7)
    {
        if(cpu->dregisters[dst - 8] == cpu->registers[src])
            cpu->flags.zero = 1;
        else if(cpu->dregisters[dst - 8] < cpu->registers[src])
        {
            cpu->flags.zero = 0;
            cpu->flags.overflow = 0;
        }   
        else if(cpu->dregisters[dst - 8] > cpu->registers[src])
        {
            cpu->flags.zero = 0;
            cpu->flags.overflow = 1;
        }
    } else if(dst > 7 && src > 7)
    {
        if(cpu->dregisters[dst - 8] == cpu->dregisters[src - 8])
            cpu->flags.zero = 1;
        else if(cpu->dregisters[dst - 8] < cpu->dregisters[src - 8])
        {
            cpu->flags.zero = 0;
            cpu->flags.overflow = 0;
        }   
        else if(cpu->dregisters[dst - 8] > cpu->dregisters[src - 8])
        {
            cpu->flags.zero = 0;
            cpu->flags.overflow = 1;
        }
    }

    cpu->PC += 2;
}

void jmp(CPU *cpu)
{
    cpu->PC = cpu->memory->code[cpu->PC+1];
}

void je(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];

    if(cpu->flags.zero == 1)
    {
        cpu->PC = dst;
    }
    else
    {
        cpu->PC += 1;
    }
}

void jne(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];

    // since not equal can be both < or > there is no need to check
    // the overflow flag...
    if(cpu->flags.zero == 0)
    {
        cpu->PC = dst;
    }
    else
    {
        cpu->PC += 1;
    }
}

void jlt(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];

    if(cpu->flags.zero == 0 && cpu->flags.overflow == 0)
    {
        cpu->PC = dst;
    }
    else
    {
        cpu->PC += 1;
    }
}

void jgt(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];

    if(cpu->flags.zero == 0 && cpu->flags.overflow == 1)
    {
        cpu->PC = dst;
    }
    else
    {
        cpu->PC += 1;
    }
}

/*
 * We load a single byte from the rwmem defined in src register, which can
 * be both a R or D register.
 * There is no immediate version of load instruction.
 */
void load(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];
    long src = cpu->memory->code[cpu->PC+2];

    if(dst <= 7 && src <= 7)
    {
        cpu->registers[dst] = cpu->memory->rwmem[cpu->registers[src]];
    } else if(dst <= 7 && src > 7)
    {
        cpu->registers[dst] =\
                cpu->memory->rwmem[(int)cpu->dregisters[src - 8]];
    } else if(dst > 7 && src <= 7)
    {
        cpu->dregisters[dst - 8] = cpu->memory->rwmem[cpu->registers[src]];
    } else if(dst > 7 && src > 7)
    {
        cpu->dregisters[dst - 8] =\
                cpu->memory->rwmem[(int)cpu->dregisters[src - 8]];
    }

    set_flags(cpu, dst);

    cpu->PC += 2;
}

/*
 * We store a single byte from the rwmem defined in src register, which can
 * be both a R or D register.
 * There is no immediate version of store instruction.
 */
void store(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];
    long src = cpu->memory->code[cpu->PC+2];

    if(dst <= 7 && src <= 7)
    {
        cpu->memory->rwmem[cpu->registers[dst]] = cpu->registers[src];
    } else if(dst <= 7 && src > 7)
    {
        cpu->memory->rwmem[(int)cpu->registers[dst]] =\
                cpu->dregisters[src - 8];
    } else if(dst > 7 && src <= 7)
    {
        cpu->memory->rwmem[(int)cpu->dregisters[dst - 8]] =\
                cpu->registers[src];
    } else if(dst > 7 && src > 7)
    {
        cpu->memory->rwmem[(int)cpu->dregisters[dst - 8]] =\
                cpu->dregisters[src - 8];
    }

    set_flags(cpu, dst);

    cpu->PC += 2;
}

/*
 * XOR operations can be done only on R<n> registers (long)
 */ 
void _xor(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];
    long src = cpu->memory->code[cpu->PC+2];

    if(dst > 7)
    {
        printf("[XORVM]::ERROR: can operate only on R<n> registers!\n");

        exit(-1);
    }

    cpu->registers[dst] ^= cpu->registers[src];

    cpu->PC += 2;
}

void xori(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];
    long src = cpu->memory->code[cpu->PC+2];

    if(dst > 7)
    {
        printf("[XORVM]::ERROR: can operate only on R<n> registers!\n");

        exit(-1);
    }

    cpu->registers[dst] ^= src;

    cpu->PC += 2;
}

/*
 * SHIFT operations can be done only on R<n> registers (long)
 */
void shl(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];
    long src = cpu->memory->code[cpu->PC+2];

    if(dst > 7)
    {
        printf("[XORVM]::ERROR: can operate only on R<n> registers!\n");

        exit(-1);
    }

    cpu->registers[dst] <<= cpu->registers[src];

    cpu->PC += 2;
}

void shli(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];
    long src = cpu->memory->code[cpu->PC+2];

    if(dst > 7)
    {
        printf("[XORVM]::ERROR: can operate only on R<n> registers!\n");

        exit(-1);
    }

    cpu->registers[dst] <<= src;

    cpu->PC += 2;
}

void shr(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];
    long src = cpu->memory->code[cpu->PC+2];

    if(dst > 7)
    {
        printf("[XORVM]::ERROR: can operate only on R<n> registers!\n");

        exit(-1);
    }

    cpu->registers[dst] >>= cpu->registers[src];

    cpu->PC += 2;
}

void shri(CPU *cpu)
{
    long dst = cpu->memory->code[cpu->PC+1];
    long src = cpu->memory->code[cpu->PC+2];

    if(dst > 7)
    {
        printf("[XORVM]::ERROR: can operate only on R<n> registers!\n");

        exit(-1);
    }

    cpu->registers[dst] >>= src;

    cpu->PC += 2;
}

/*
 * CPU utility functions implementation
 */

void print_cpu_registers(CPU *cpu)
{
    printf("[REGISTERS]\n");

    for(size_t i = 0; i < 8; ++i)
    {
        printf("R%ld: %ld\n", i, cpu->registers[i]);
    }

    for(size_t i = 0; i < 8; ++i)
    {
        printf("D%ld: %.15f\n", i, cpu->dregisters[i]);
    }

    printf("\n");
}

void print_cpu_flags(CPU *cpu)
{
    printf("[FLAGS]\n");
    
    printf("ZERO        : %d\n", cpu->flags.zero);
    printf("NEGATIVE    : %d\n", cpu->flags.negative);
    printf("OVERFLOW    : %d\n", cpu->flags.overflow);

    printf("\n");
}
