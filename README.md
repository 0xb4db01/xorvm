# XORVM

## Overview

XORVM is a emulated CPU with a custom instruction set that I initially made (for learning purposes) to create a XOR encryption function that is hard to reverse engineer.

Since I was kinda happy with it I decided to add more instructions other than just XOR, so maybe I could implement more "complex" encryption functions. Now here is the thing, since I added operation instructions such as ADD, SUB etc... i felt bad not adding DIV and I also felt bad adding DIV only on integer numbers, so I decided to add 8 double/float registers just for the sake of floating point numbers. This is also the reason why the code to be executed must be defined in a double array, so one can use double numbers as immediate values.

All of this seems kinda useless when I think about the initial purpose of the VM and I'm afraid the implementation that I ended up with is not the best choice i've made in my life, but I don't care at this point since I reached my goal, which was not a beauty contest.

Finally, you are free to use this for educational purposes only. I am not responsible if you're a skid and do stupid shit with my code.

## Usage and compiling

To use XORVM you just need to include cpu.h in your code. Keep in mind that all the .h files in src/ must be there even if you only need cpu.h.

If you compile this code a build/release/tests executable is generated *and* executed by make. Hopefully tests will go well but keep in mind I shall rewrite them better, mainly because the tests with double numbers may fail due to floating point precision (I don't know how this stuff is likely to change from system to system...).

Also, I am too lazy to write some good documentation, hopefully src/tests.c will help.

Some code using XORVM may be something like the following:

```
void A_to_B()
{
    unsigned char rwmem[] = {'A'};

    double code[] = {
        // all registers should be zero at the very beginning...
        LD, R0, R1, // load rwmem[0] in R0 (R1 == 0)
        ADDI, R0, 1 // add 1 to R0 ('A' == 65, so now it's 66)
        STR, R1, R0 // store 66 in rwmem[0] (R1 still == 0)
        HLT // end of code
    };

    Memory memory;
    memory.code_size = sizeof(code);
    memory.code = code;
    memory.rwmem_size = 1;
    memory.rwmem = rwmem;

    CPU *cpu = new_cpu(&memory);

    run_cpu(cpu);

    free_cpu(cpu);

    // do whatever you want with rwmem now that it has changed...
}
```

Note that the order in all instructions is always:

```
<INSTRUCTION/I> <DESTINATION REGISTER> <SOURCE REGISTER/VALUE>
```

## CPU Flags

XORVM CPU implements the following flags:

- Zero
- Negative
- Overflow

## Instructions

This is the instructions definition

```
enum Instructions
{
    MOV, MOVI,
    ADD, ADDI, SUB, SUBI, MUL, MULI, DIV, DIVI,
    CMP,
    JMP, JE, JNE, JLT, JGT,
    LD, STR,
    XOR, XORI, SHL, SHR, SHLI, SHRI,
    HLT
};
```

As you can see, there are many "i" instructions, where "i" obviously stands for "immediate", meaning you can MOVI R<n> 1 for example. Non-immediate instructions clearly require to operate from a register to another.

### Operation instructions

```
ADD, ADDI, SUB, SUBI, MUL, MULI, DIV, DIVI,
```

These are the classi addition, substraction, multiplication and division instructions, with their immediate variants.

It is possible to operate from R registers to D registers and vice versa, keeping in mind that you will loose (or gain) floating point whenever you do so.

### Compare instruction

Comparing sets flags according to the results.

```
DST == SRC --> flag.zero = 1
DST < SRC  --> flag.zero = 0, flag.overflow = 0
DST > SRC  --> flag.zero = 0, flag.overflow = 1
```
This should be sufficient for loops.

### Jump instructions

There are only 5 possible jumps

```
JMP, JE, JNE, JLT, JGT,
```

Direct jump, jump if equale, jump if not equal, jump if less than and jump if greater than. There is no "jump if greater/less or equal than", keep this in mind when you create loops.

### RAM Access instructions

XORVM's Memory must be defined when creating a new CPU. The read/write memory can be allocated or can be a pointer of some other memory defined previously in your code. There is no direct access to ram, you must use the LD (load) and STR (store) instructions, which have no immediate variants.

So, for example if you want to do something with whatever is in cpu->memory->rwmem[1] you will need to do something like

```
MOVI, R1, 1,
LD, R0, R1
```

This will load in R0 whatever is in cpu->memory->rwmem[1] and you can do whatever you want with it. Once you're done, you can store it back. For example, assuming your modified byte is in R0:

```
MOVI, R1, 1,
STD, R1, R0
```

### Bitwise operators

At the moment, only the following are supported:

```
XOR, XORI, SHL, SHR, SHLI, SHRI,
```

These are xor, shift left, shift right and their immediate variants.

## Looping

Loops are possible obviously, keeping in mind these things:

- There are no labels, so when you jump conditionally, you must jump at the position just before the instruction you want to execute. 
- There is no increment instruction, use ADD/ADDI.

Consider the following code:

```
double code[] = {
    MOVI, R1, 3,
    ADDI, R2, 1,
    CMP, R2, R1,
    JNE, 2,

    HLT
};
```

This loops incrementing R2 until it's not equal to the content of R1, which is 3. Meaning at the end of the loop R2 == 3.

The source file src/tests.c contains a couple of jump test functions and a simple functions that encrypts an array with xor and a single byte as key. Hopefully this might be of use as an example.
