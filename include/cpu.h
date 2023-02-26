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
    bool enabling_mie;
    u8 ie_register;
    u8 interrupt_flag;
} cpu_context;

void cpu_init();
bool cpu_run();

cpu_registers *get_register();
u16 register_read(register_type rt);
void register_set(register_type rt, u16 n);

u8 reg8_read(register_type rt);
void reg8_set(register_type rt, u8 v);

u8 ie_register_get();
void ie_register_set(u8 n);

u8 interrupt_flag_get();
void interrupt_flag_set(u8 n);

typedef void (*IN_PROCESS)(cpu_context *);
IN_PROCESS inst_get_processor(instruction_type type);

void disassemble(cpu_context *ctx, char *str);

//F - Flag Register
#define FLAG_Z BIT(ctx->regs.f, 7) //Zero
#define FLAG_S BIT(ctx->regs.f, 6) //Sub
#define FLAG_H BIT(ctx->regs.f, 5) //Half-Carry 
#define FLAG_C BIT(ctx->regs.f, 4) //Carry
