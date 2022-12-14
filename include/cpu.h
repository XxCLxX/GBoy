#pragma once
#include <common.h>
#include <cpu_instruct.h>

typedef struct
{
    u8 a; // Accumulator
    u8 f; // Flag
    u8 b;
    u8 c;
    u8 d;
    u8 e;
    u8 h;
    u8 l;
    u16 pc; // Program Counter
    u16 sp; // Stack Pointer
} cpu_registers;

typedef struct
{
    cpu_registers regs;
    u16 fetch_data;
    u16 memory_dest;
    bool dest_is_memory;
    u8 cur_opcode;
    instruction *cur_instruct;
    bool halt;
    bool step;
} cpu_context;

void cpu_init();
bool cpu_run();
