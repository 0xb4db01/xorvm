#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "cpu.h"

void test_mov()
{
    printf("[+] TESTING MOV/I INSTRUCTIONS... ");

    double code[] = {
        MOVI, R0, 1,
        MOV, R1, R0,
        MOVI, D0, 7.64,
        MOV, D1, R1,
        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->registers[0] == 1);
    assert(cpu->registers[1] == 1);
    assert(cpu->dregisters[0] == 7.64);
    assert(cpu->dregisters[1] == 1.0);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_add()
{
    printf("[+] TESTING ADD/I INSTRUCTIONS... ");

    double code[] = {
        MOVI, D0, 3.14,
        MOVI, D1, 4.5,
        ADD, D0, D1,
        MOV, D2, D1,
        ADDI, D2, 1.0,
        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->dregisters[0] == 7.640000000000001);
    assert(cpu->dregisters[2] == 5.5);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_sub()
{
    printf("[+] TESTING SUB/I INSTRUCTIONS... ");

    double code[] = {
        MOVI, R0, 10,
        MOVI, R1, 4,

        // substract 2 long registers
        SUB, R0, R1,

        MOVI, D0, 4.5,
        MOVI, D1, 3.14,

        // substract 2 double registers
        SUB, D0, D1,

        MOV, D2, R0,

        // substract a long register from a double register
        SUB, D2, R1,

        MOV, D3, D2,

        // substract immediate value from double register
        SUBI, D3, 1.5,

        MOV, R3, R1,

        // substract a double register from a long register
        SUB, R3, D0,

        // test negative result on long register
        SUBI, R4, 8,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->registers[0] == 6);
    assert(cpu->dregisters[0] == 1.3599999999999999);
    assert(cpu->dregisters[2] == 2.0);
    assert(cpu->dregisters[3] == 0.5);
    assert(cpu->registers[3] == 2);
    assert(cpu->flags.negative == 1);

    free_cpu(cpu);

    printf("OK!\n");
}


void test_mul()
{
    printf("[+] TESTING MUL/I INSTRUCTIONS... ");

    double code[] = {
        MOVI, R0, 10,
        MOVI, R1, 4,

        // multiply 2 long registers
        MUL, R0, R1,

        MOVI, D0, 4.5,
        MOVI, D1, 3.14,

        // multiply 2 double registers
        MUL, D0, D1,

        MOV, D2, R0,

        // multiply a long register from a double register
        MUL, D2, R1,

        MOV, D3, D2,

        // multiply immediate value from double register
        MULI, D3, 1.5,

        MOV, R3, R1,

        // multiply a double register from a long register
        MUL, R3, D0,

        // test negative result on long zeroed register
        MULI, R4, 8,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->registers[0] == 40);
    assert(cpu->dregisters[0] == 14.130000000000001);
    assert(cpu->dregisters[2] == 160.000000000000000);
    assert(cpu->registers[3] == 56);
    assert(cpu->registers[4] == 0);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_div()
{
    printf("[+] TESTING DIV/I INSTRUCTIONS... ");

    double code[] = {
        MOVI, R0, 10,
        MOVI, R1, 4,

        // divide 2 long registers
        DIV, R0, R1,

        MOVI, D0, 4.5,
        MOVI, D1, 3.14,

        // divide 2 double registers
        DIV, D0, D1,

        MOV, D2, R0,

        // divide a long register from a double register
        DIV, D2, R1,

        MOV, D3, D2,

        // divide immediate value from double register
        DIVI, D3, 1.5,

        MOV, R3, R1,

        // divide a double register from a long register
        DIV, R3, D0,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->registers[0] == 2);
    assert(cpu->dregisters[0] == 1.4331210191082802);
    assert(cpu->dregisters[2] == 0.5);
    assert(cpu->dregisters[3] == 0.33333333333333331);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_xor()
{
    printf("[+] TESTING XOR/I INSTRUCTIONS... ");

    double code[] = {
        MOVI, R0, 65,
        MOVI, R1, 0xa,
        XOR, R0, R1,
        MOV, R2, R0,
        XORI, R2, 0xa,
        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->registers[0] == 75);
    assert(cpu->registers[2] == 65);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_shl()
{
    printf("[+] TESTING SHL/I INSTRUCTIONS... ");

    double code[] = {
        MOVI, R0, 4,
        MOVI, R1, 1,
        SHL, R0, R1,
        MOV, R2, R0,
        SHLI, R2, 2,
        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->registers[0] == 8);
    assert(cpu->registers[2] == 32);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_shr()
{
    printf("[+] TESTING SHR/I INSTRUCTIONS... ");

    double code[] = {
        MOVI, R0, 4,
        MOVI, R1, 1,
        SHR, R0, R1,
        MOV, R2, R0,
        SHRI, R2, 1,
        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->registers[0] == 2);
    assert(cpu->registers[2] == 1);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_load()
{
    printf("[+] TESTING LD INSTRUCTION... ");

    unsigned char payload[] = {'A', 'B', 'C', 'D'};

    double code[] = {
        // test load from R<n> to R<n> register
        LD, R0, R1,

        // test load from D<n> to R<n> register
        MOVI, R1, 1,
        LD, D0, R1,

        // test load from R<n> to D<n> register
        MOVI, D1, 2,
        LD, R2, D1,

        // test load from D<n> to D<n> register
        MOVI, D1, 3,
        LD, D2, D1,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = payload;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->registers[0] == 'A');
    assert(cpu->dregisters[0] == 'B');
    assert(cpu->registers[2] == 'C');
    assert(cpu->dregisters[2] == 'D');

    free_cpu(cpu);

    printf("OK!\n");
}

void test_store()
{
    printf("[+] TESTING STR INSTRUCTION... ");

    unsigned char payload[] = {'X', 'X', 'X', 'X'};

    double code[] = {
        // test store from R<n> to R<n> register
        MOVI, R1, 65,
        STR, R0, R1,

        // test store from D<n> to R<n> register
        MOVI, R0, 1,
        MOVI, D0, 66,
        STR, R0, D0,

        // test store from R<n> to D<n> register
        MOVI, D1, 2,
        MOVI, R2, 67,
        STR, D1, R2,

        // test store from D<n> to D<n> register
        MOVI, D2, 3,
        MOVI, D3, 68,
        STR, D2, D3,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = payload;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(payload[0] == 'A');
    assert(payload[1] == 'B');
    assert(payload[2] == 'C');
    assert(payload[3] == 'D');

    free_cpu(cpu);

    printf("OK!\n");
}

void test_cmp_equal()
{
    printf("[+] TESTING CMP EQUAL INSTRUCTION... ");

    double code[] = {
        MOVI, R0, 1,
        MOVI, R1, 1,
        CMP, R0, R1,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->flags.zero == 1);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_cmp_not_equal()
{
    printf("[+] TESTING CMP NOT EQUAL INSTRUCTION... ");

    double code[] = {
        MOVI, R0, 1,
        MOVI, R1, 2,
        CMP, R0, R1,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->flags.zero == 0);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_cmp_less_than()
{
    printf("[+] TESTING CMP LESS THAN INSTRUCTION... ");

    double code[] = {
        MOVI, R0, 1,
        MOVI, R1, 2,
        CMP, R0, R1,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->flags.zero == 0);
    assert(cpu->flags.overflow == 0);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_cmp_greater_than()
{
    printf("[+] TESTING CMP GREATER THAN INSTRUCTION... ");

    double code[] = {
        MOVI, R0, 2,
        MOVI, R1, 1,
        CMP, R0, R1,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->flags.zero == 0);
    assert(cpu->flags.overflow == 1);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_je()
{
    printf("[+] TESTING JE INSTRUCTION... ");

    double code[] = {
        MOVI, R1, 3,
        ADDI, R2, 1,
        CMP, R2, R1,
        JE, 12,
        JMP, 2,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->registers[2] == 3);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_jne()
{
    printf("[+] TESTING JNE INSTRUCTION... ");

    double code[] = {
        MOVI, R1, 3,
        ADDI, R2, 1,
        CMP, R2, R1,
        JNE, 2,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->registers[2] == 3);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_jlt()
{
    printf("[+] TESTING JLT INSTRUCTION... ");

    double code[] = {
        MOVI, R1, 3,
        ADDI, R2, 1,
        CMP, R2, R1,
        JLT, 2,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->registers[2] == 3);

    free_cpu(cpu);

    printf("OK!\n");
}

void test_jgt()
{
    printf("[+] TESTING JLT INSTRUCTION... ");

    double code[] = {
        MOVI, R1, 3,
        ADDI, R2, 1,
        CMP, R2, R1,
        JGT, 12,
        JMP, 2,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = NULL;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    assert(cpu->registers[2] == 4);

    free_cpu(cpu);

    printf("OK!\n");
}

void xorfun()
{
    printf("\n[+] A MORE COMPLEX XOR ENCRYPTION FUNCTION...\n");

    unsigned char payload1[] = {'A', 'B', 'C', 'D', 0};
    unsigned char payload2[] = {'A', 'B', 'C', 'D', 0};

    double code[] = {
        // payload length
        MOVI, R2, 4,

        // xor key
        MOVI, R3, 0x12,

        LD, R0, R1,
        XOR, R0, R3,
        STR, R1, R0,
        ADDI, R1, 1,
        CMP, R1, R2,
        JNE, 5,

        HLT
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 5;
    memory.rwmem = payload1;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    printf("Original payload       : %s\n", payload2);
    printf("XORd payload (key 0x12): %s\n", payload1);

    free_cpu(cpu);
}

int main()
{
    printf("[+] RUNNING TESTS...\n\n");

    test_mov();
    test_add();
    test_sub();
    test_mul();
    test_div();
    test_xor();
    test_shl();
    test_shr();
    test_load();
    test_store();
    test_cmp_equal();
    test_cmp_not_equal();
    test_cmp_less_than();
    test_cmp_greater_than();
    test_je();
    test_jne();
    test_jlt();
    test_jgt();

    printf("\n[+] ALL TESTS OK\n");

    xorfun();
}
