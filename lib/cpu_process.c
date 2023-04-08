#include <cpu.h>
#include <gboy.h>
#include <bus.h>
#include <stack.h>

// Flags: Z - S - H - C
void set_flags(cpu_context *ctx, int8_t z, int8_t s, int8_t h, int8_t c)
{
    if (z != -1)
    {
        BIT_SET(ctx->regs.f, 7, z);
    }

    if (s != -1)
    {
        BIT_SET(ctx->regs.f, 6, s);
    }

    if (h != -1)
    {
        BIT_SET(ctx->regs.f, 5, h);
    }

    if (c != -1)
    {
        BIT_SET(ctx->regs.f, 4, c);
    }
}

static void proc_none(cpu_context *ctx)
{
    printf("INVALID INSTRUCTION %2X\n", ctx->cur_opcode);
    exit(-7);
}

static void proc_nop(cpu_context *ctx)
{
}

register_type rt_lookup[] = {
    RT_B,
    RT_C,
    RT_D,
    RT_E,
    RT_H,
    RT_L,
    RT_HL,
    RT_A};

register_type decode_rt(u8 reg)
{
    if (reg > 0b111)
    {
        return RT_NONE;
    }
    return rt_lookup[reg];
}

static void proc_and(cpu_context *ctx)
{
    ctx->regs.a &= ctx->fetch_data;
    set_flags(ctx, ctx->regs.a == 0, 0, 1, 0);
}

static void proc_or(cpu_context *ctx)
{
    ctx->regs.a |= ctx->fetch_data & 0xFF;
    set_flags(ctx, ctx->regs.a == 0, 0, 0, 0);
}

static void proc_xor(cpu_context *ctx)
{
    ctx->regs.a ^= ctx->fetch_data & 0xFF;
    set_flags(ctx, ctx->regs.a == 0, 0, 0, 0);
}

static void proc_cp(cpu_context *ctx)
{
    int n = (int)ctx->regs.a - (int)ctx->fetch_data;

    set_flags(ctx, n == 0, 1, ((int)ctx->regs.a & 0x0F) - ((int)ctx->fetch_data & 0x0F) < 0, n < 0);
}

static void proc_cb(cpu_context *ctx)
{
    u8 op = ctx->fetch_data;
    register_type reg = decode_rt(op & 0b111);
    u8 bit = (op >> 3) & 0b111;
    u8 bit_op = (op >> 6) & 0b11;
    u8 reg_value = reg8_read(reg);

    gboy_cycles(1);
    if (reg == RT_HL)
    {
        gboy_cycles(2);
    }

    switch (bit_op)
    {
        // BIT
    case 1:
        set_flags(ctx, !(reg_value & (1 << bit)), 0, 1, -1);
        return;

        // RST
    case 2:
        reg_value &= ~(1 << bit);
        reg8_set(reg, reg_value);
        return;

        // SET
    case 3:
        reg_value |= (1 << bit);
        reg8_set(reg, reg_value);
        return;
    }

    bool flagC = FLAG_C;

    // Rotate Shift Instructions in Prefic CB
    switch (bit)
    {
    // RLC
    case 0:
    {
        bool setC = false;
        u8 res = (reg_value << 1) & 0xFF;
        if ((reg_value & (1 << 7)) != 0)
        {
            res |= 1;
            setC = true;
        }

        reg8_set(reg, res);
        set_flags(ctx, res == 0, false, false, setC);
    }
        return;

    // RRC
    case 1:
    {
        u8 old = reg_value;
        reg_value >>= 1;
        reg_value |= (old << 7);

        reg8_set(reg, reg_value);
        set_flags(ctx, !reg_value, false, false, old & 1);
    }
        return;

    // RL - Rotate Left
    case 2:
    {
        u8 old = reg_value;
        reg_value <<= 1;
        reg_value |= flagC;

        reg8_set(reg, reg_value);
        set_flags(ctx, !reg_value, false, false, !!(old & 0x80));
    }
        return;

    // RR - Rotate Right
    case 3:
    {
        u8 old = reg_value;
        reg_value >>= 1;
        reg_value |= (flagC << 7);

        reg8_set(reg, reg_value);
        set_flags(ctx, !reg_value, false, false, old & 1);
    }
        return;

    // SLA
    case 4:
    {
        u8 old = reg_value;
        reg_value <<= 1;

        reg8_set(reg, reg_value);
        set_flags(ctx, !reg_value, false, false, !!(old & 0x80));
    }
        return;

    // SRA
    case 5:
    {
        u8 u = (int8_t)reg_value >> 1;
        reg8_set(reg, u);
        set_flags(ctx, !u, 0, 0, reg_value & 1);
    }
        return;

    // SWAP
    case 6:
    {
        reg_value = ((reg_value & 0xF0) >> 4) | ((reg_value & 0xF) << 4);
        reg8_set(reg, reg_value);
        set_flags(ctx, reg_value == 0, false, false, false);
    }
        return;

    // SRL
    case 7:
    {
        u8 u = reg_value >> 1;
        reg8_set(reg, u);
        set_flags(ctx, !u, 0, 0, reg_value & 1);
    }
        return;
    }
    fprintf(stderr, "ERROR: Invalid CB: %02X", op);
    NO_IMPLEM
}

// General-Purpose Arthermetic operations & CPU Control Instructions
static void proc_stop(cpu_context *ctx)
{
    fprintf(stderr, "STOP\n");
    NO_IMPLEM
}

static void proc_daa(cpu_context *ctx)
{
    u8 u = 0;
    int flagC = 0;

    if (FLAG_H || (!FLAG_S && (ctx->regs.a & 0xF) > 9))
    {
        u = 6;
    }

    if (FLAG_C || (!FLAG_S && ctx->regs.a > 0x99))
    {
        u |= 0x60;
        flagC = 1;
    }

    ctx->regs.a += FLAG_S ? -u : u;
    set_flags(ctx, ctx->regs.a == 0, -1, 0, flagC);
}

static void proc_cpl(cpu_context *ctx)
{
    ctx->regs.a = ~ctx->regs.a;
    set_flags(ctx, -1, 1, 1, -1);
}

static void proc_scf(cpu_context *ctx)
{
    set_flags(ctx, -1, 0, 0, 1);
}

static void proc_ccf(cpu_context *ctx)
{
    set_flags(ctx, -1, 0, 0, FLAG_C ^ 1);
}

// Rotate Shift Instructions
static void proc_rla(cpu_context *ctx)
{
    u8 u = ctx->regs.a;
    u8 flagC = FLAG_C;
    u8 c = (u >> 7) & 1;

    ctx->regs.a = (u << 1) | flagC;
    set_flags(ctx, 0, 0, 0, c);
}

static void proc_rra(cpu_context *ctx)
{
    u8 carry = FLAG_C;
    u8 c = ctx->regs.a & 1;

    ctx->regs.a >>= 1;
    ctx->regs.a |= (carry << 7);

    set_flags(ctx, 0, 0, 0, c);
}

static void proc_rlca(cpu_context *ctx)
{
    u8 u = ctx->regs.a;
    bool flagC = (u >> 7) & 1;
    u = (u << 1) | flagC;
    ctx->regs.a = u;

    set_flags(ctx, 0, 0, 0, flagC);
}

static void proc_rrca(cpu_context *ctx)
{
    u8 c = ctx->regs.a & 1;
    ctx->regs.a >>= 1;
    ctx->regs.a |= (c << 7);

    set_flags(ctx, 0, 0, 0, c);
}

static void proc_halt(cpu_context *ctx)
{
    ctx->halt = true;
}

static void proc_di(cpu_context *ctx)
{
    ctx->master_interrupt_enabled = false;
}

static void proc_ei(cpu_context *ctx)
{
    ctx->enabling_mie = true;
}

static bool is_16bit(register_type rt)
{
    return rt >= RT_AF;
}

// Load function
static void proc_ld(cpu_context *ctx)
{
    if (ctx->dest_is_memory)
    {
        if (is_16bit(ctx->cur_instruct->reg_2)) // if 16-bit register
        {
            gboy_cycles(1);
            bus_write16(ctx->memory_dest, ctx->fetch_data);
        }
        else
        {
            bus_write(ctx->memory_dest, ctx->fetch_data);
        }
        gboy_cycles(1);
        return;
    }

    if (ctx->cur_instruct->mode == AM_HL_SPR)
    {
        u8 hflag = (register_read(ctx->cur_instruct->reg_2) & 0xF) + (ctx->fetch_data & 0xF) >= 0x10;

        u8 cflag = (register_read(ctx->cur_instruct->reg_2) & 0xFF) + (ctx->fetch_data & 0xFF) >= 0x100;

        set_flags(ctx, 0, 0, hflag, cflag);
        register_set(ctx->cur_instruct->reg_1, register_read(ctx->cur_instruct->reg_2) + (int8_t)ctx->fetch_data);

        return;
    }
    register_set(ctx->cur_instruct->reg_1, ctx->fetch_data);
}

// Caused issue of bugs
static void proc_ldh(cpu_context *ctx)
{
    if (ctx->cur_instruct->reg_1 == RT_A)
    {
        register_set(ctx->cur_instruct->reg_1, bus_read(0xFF00 | ctx->fetch_data));
    }
    else
    {
        // bus_write(0xFF00 | ctx->fetch_data, ctx->regs.a);
        bus_write(ctx->memory_dest, ctx->regs.a);
    }

    gboy_cycles(1);
}

// Conditional jump if nz,z,nc,c
static bool check_condition(cpu_context *ctx)
{
    bool z = FLAG_Z;
    bool c = FLAG_C;

    switch (ctx->cur_instruct->cond)
    {
    case CT_NONE:
        return true;
    case CT_NZ:
        return !z;
    case CT_Z:
        return z;
    case CT_NC:
        return !c;
    case CT_C:
        return c;
    }
    return false;
}

static void goto_address(cpu_context *ctx, u16 address, bool pushpc)
{
    if (check_condition(ctx))
    {
        if (pushpc)
        {
            gboy_cycles(2); // 2 because its 16 bit address
            stack_push16(ctx->regs.pc);
        }

        ctx->regs.pc = address;
        gboy_cycles(1);
    }
}

// Jump function, doesn't push the pc
static void proc_jp(cpu_context *ctx)
{
    goto_address(ctx, ctx->fetch_data, false);
}

// Jump relative function
static void proc_jr(cpu_context *ctx)
{
    int8_t rel = (int8_t)(ctx->fetch_data & 0xFF);
    u16 address = ctx->regs.pc + rel;
    goto_address(ctx, address, false);
}

// Call function, does push the pc
static void proc_call(cpu_context *ctx)
{
    goto_address(ctx, ctx->fetch_data, true);
}

// Restart function
static void proc_rst(cpu_context *ctx)
{
    goto_address(ctx, ctx->cur_instruct->param, true);
}

// Return call function
static void proc_ret(cpu_context *ctx)
{
    if (ctx->cur_instruct->cond != CT_NONE)
    {
        gboy_cycles(1);
    }

    if (check_condition(ctx))
    {
        u16 low = stack_pop();
        gboy_cycles(1);
        u16 high = stack_pop();
        gboy_cycles(1);

        u16 n = (high << 8) | low;
        ctx->regs.pc = n;

        gboy_cycles(1);
    }
}

static void proc_reti(cpu_context *ctx)
{
    ctx->master_interrupt_enabled = true;
    proc_ret(ctx);
}

static void proc_pop(cpu_context *ctx)
{
    u16 low = stack_pop();
    gboy_cycles(1);
    u16 high = stack_pop();
    gboy_cycles(1);

    u16 n = (high << 8) | low;
    register_set(ctx->cur_instruct->reg_1, n);

    if (ctx->cur_instruct->reg_1 == RT_AF)
    {
        register_set(ctx->cur_instruct->reg_1, n & 0xFFF0);
    }
}

static void proc_push(cpu_context *ctx)
{
    u16 high = (register_read(ctx->cur_instruct->reg_1) >> 8) & 0xFF;
    gboy_cycles(1);
    stack_push(high);

    u16 low = register_read(ctx->cur_instruct->reg_1) & 0xFF;
    gboy_cycles(1);
    stack_push(low);

    gboy_cycles(1);
}

// Increment function
static void proc_inc(cpu_context *ctx)
{
    u16 v = register_read(ctx->cur_instruct->reg_1) + 1;

    if (is_16bit(ctx->cur_instruct->reg_1))
    {
        gboy_cycles(1);
    }

    if (ctx->cur_instruct->reg_1 == RT_HL && ctx->cur_instruct->mode == AM_MR)
    {
        v = bus_read(register_read(RT_HL)) + 1;
        v &= 0xFF;
        bus_write(register_read(RT_HL), v);
    }
    else
    {
        register_set(ctx->cur_instruct->reg_1, v);
        v = register_read(ctx->cur_instruct->reg_1);
    }

    if ((ctx->cur_opcode & 0x03) == 0x03)
    {
        return;
    }

    set_flags(ctx, v == 0, 0, (v & 0x0F) == 0, -1);
}

// Decrement function
static void proc_dec(cpu_context *ctx)
{
    u16 v = register_read(ctx->cur_instruct->reg_1) - 1;

    if (is_16bit(ctx->cur_instruct->reg_1))
    {
        gboy_cycles(1);
    }

    if (ctx->cur_instruct->reg_1 == RT_HL && ctx->cur_instruct->mode == AM_MR)
    {
        v = bus_read(register_read(RT_HL)) - 1;
        // v &= 0xFF;
        bus_write(register_read(RT_HL), v);
    }
    else
    {
        register_set(ctx->cur_instruct->reg_1, v);
        v = register_read(ctx->cur_instruct->reg_1);
    }

    if ((ctx->cur_opcode & 0x0B) == 0x0B)
    {
        return;
    }

    set_flags(ctx, v == 0, 1, (v & 0x0F) == 0x0F, -1);
}

// Add function
static void proc_add(cpu_context *ctx)
{
    u32 v = register_read(ctx->cur_instruct->reg_1) + ctx->fetch_data;
    bool check_16bit = is_16bit(ctx->cur_instruct->reg_1);

    if (check_16bit)
    {
        gboy_cycles(1);
    }

    if (ctx->cur_instruct->reg_1 == RT_SP)
    {
        v = register_read(ctx->cur_instruct->reg_1) + (int8_t)ctx->fetch_data;
    }

    int z = (v & 0xFF) == 0;
    int h = (register_read(ctx->cur_instruct->reg_1) & 0xF) + (ctx->fetch_data & 0xF) >= 0x10;
    int c = (int)(register_read(ctx->cur_instruct->reg_1) & 0xFF) + (int)(ctx->fetch_data & 0xFF) >= 0x100;

    if (check_16bit)
    {
        z = -1;
        h = (register_read(ctx->cur_instruct->reg_1) & 0xFFF) + (ctx->fetch_data & 0xFFF) >= 0x1000;
        u32 n = ((u32)register_read(ctx->cur_instruct->reg_1)) + ((u32)ctx->fetch_data);
        c = n >= 0x10000;
    }

    if (ctx->cur_instruct->reg_1 == RT_SP)
    {
        z = 0;
        h = (register_read(ctx->cur_instruct->reg_1) & 0xF) + (ctx->fetch_data & 0xF) >= 0x10;
        c = (int)(register_read(ctx->cur_instruct->reg_1) & 0xFF) + (int)(ctx->fetch_data & 0xFF) >= 0x100;
    }

    register_set(ctx->cur_instruct->reg_1, v & 0xFFFF);
    set_flags(ctx, z, 0, h, c);
}

// Function Add with carry
static void proc_adc(cpu_context *ctx)
{
    u16 u = ctx->fetch_data;
    u16 a = ctx->regs.a;
    u16 c = FLAG_C;

    ctx->regs.a = (a + u + c) & 0xFF;

    set_flags(ctx, ctx->regs.a == 0, 0,
              (a & 0xF) + (u & 0xF) + c > 0xF,
              a + u + c > 0xFF);
}

// Function Subtraction
static void proc_sub(cpu_context *ctx)
{
    u16 v = register_read(ctx->cur_instruct->reg_1) - ctx->fetch_data;

    int z = v == 0;
    int h = ((int)register_read(ctx->cur_instruct->reg_1) & 0xF) - ((int)ctx->fetch_data & 0xF) < 0;
    int c = ((int)register_read(ctx->cur_instruct->reg_1)) - ((int)ctx->fetch_data) < 0;

    register_set(ctx->cur_instruct->reg_1, v);
    set_flags(ctx, z, 1, h, c);
}

// Function Subtraction with carry
static void proc_sbc(cpu_context *ctx)
{
    u8 v = ctx->fetch_data + FLAG_C;

    int z = register_read(ctx->cur_instruct->reg_1) - v == 0;
    int h = ((int)register_read(ctx->cur_instruct->reg_1) & 0xF) - ((int)ctx->fetch_data & 0xF) - ((int)FLAG_C) < 0;
    int c = ((int)register_read(ctx->cur_instruct->reg_1)) - ((int)ctx->fetch_data) - ((int)FLAG_C) < 0;

    register_set(ctx->cur_instruct->reg_1, register_read(ctx->cur_instruct->reg_1) - v);
    set_flags(ctx, z, 1, h, c);
}

static IN_PROCESS processors[] =
    {
        [IN_NONE] = proc_none,
        [IN_NOP] = proc_nop,
        [IN_LD] = proc_ld,
        [IN_LDH] = proc_ldh,
        [IN_POP] = proc_pop,
        [IN_PUSH] = proc_push,
        [IN_JP] = proc_jp,
        [IN_JR] = proc_jr,
        [IN_CALL] = proc_call,
        [IN_RET] = proc_ret,
        [IN_RETI] = proc_reti,
        [IN_RST] = proc_rst,
        [IN_INC] = proc_inc,
        [IN_DEC] = proc_dec,
        [IN_ADD] = proc_add,
        [IN_ADC] = proc_adc,
        [IN_SUB] = proc_sub,
        [IN_SBC] = proc_sbc,
        [IN_DI] = proc_di,
        [IN_XOR] = proc_xor,
        [IN_AND] = proc_and,
        [IN_OR] = proc_or,
        [IN_CP] = proc_cp,
        [IN_CB] = proc_cb,
        [IN_RRA] = proc_rra,
        [IN_RLA] = proc_rla,
        [IN_RRCA] = proc_rrca,
        [IN_RLCA] = proc_rlca,
        [IN_STOP] = proc_stop,
        [IN_HALT] = proc_halt,
        [IN_DAA] = proc_daa,
        [IN_CPL] = proc_cpl,
        [IN_SCF] = proc_scf,
        [IN_CCF] = proc_ccf,
        [IN_EI] = proc_ei};

IN_PROCESS inst_get_processor(instruction_type type)
{
    return processors[type];
}