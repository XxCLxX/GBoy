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
    bool master_interrupt_enabled;
} cpu_context;

void cpu_init();
bool cpu_run();

u16 register_read(register_type rt);
void register_set(register_type rt, u16 n);

typedef void (*IN_PROCESS)(cpu_context *);
IN_PROCESS inst_get_processor(instruction_type type);

//F - Flag Register
#define FLAG_Z BIT(ctx->regs.f, 7) //Zero
#define FLAG_S BIT(ctx->regs.f, 6) //Sub
#define FLAG_H BIT(ctx->regs.f, 5) //Half-Carry 
#define FLAG_C BIT(ctx->regs.f, 4) //Carry
