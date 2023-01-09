#include <cpu.h>
#include <gboy.h>
#include <bus.h>
#include <stack.h>

// Flags: Z - S - H - C
void set_flags(cpu_context *ctx, char z, char s, char h, char c)
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
    printf("INVALID INSTRUCTION\n");
    exit(-7);
}

static void proc_nop(cpu_context *ctx)
{
}

static void proc_di(cpu_context *ctx)
{
    ctx->master_interrupt_enabled = false;
}

// Load function
static void proc_ld(cpu_context *ctx)
{
    if (ctx->dest_is_memory)
    {
        if (ctx->cur_instruct->reg_2 >= RT_AF) // if 16-bit register
        {
            gboy_cycles(1);
            bus_write16(ctx->memory_dest, ctx->fetch_data);
        }
        else
        {
            bus_write(ctx->memory_dest, ctx->fetch_data);
        }
        return;
    }

    if (ctx->cur_instruct->mode == AM_HL_SPR)
    {
        u8 hflag = (register_read(ctx->cur_instruct->reg_2) & 0xF) + (ctx->fetch_data & 0xF) >= 0x10;

        u8 cflag = (register_read(ctx->cur_instruct->reg_2) & 0xFF) + (ctx->fetch_data & 0xFF) >= 0x100;

        set_flags(ctx, 0, 0, hflag, cflag);
        register_set(ctx->cur_instruct->reg_1, register_read(ctx->cur_instruct->reg_2) + (char)ctx->fetch_data);

        return;
    }
    register_set(ctx->cur_instruct->reg_1, ctx->fetch_data);
}

static void proc_ldh(cpu_context *ctx)
{
    if (ctx->cur_instruct->reg_1 == RT_A)
    {
        register_set(ctx->cur_instruct->reg_1, bus_read(0xFF00 | ctx->fetch_data));
    }
    else
    {
        bus_write(0xFF00 | ctx->fetch_data, ctx->regs.a);
    }

    gboy_cycles(1);
}

static void proc_xor(cpu_context *ctx)
{
    ctx->regs.a ^= ctx->fetch_data & 0xFF;
    set_flags(ctx, ctx->regs.a == 0, 0, 0, 0);
}

// conditional jump if nz,z,nc,c
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

static void proc_jp(cpu_context *ctx) // Jump function, doesn't push the pc
{
    goto_address(ctx, ctx->fetch_data, false);
}

static void proc_jr(cpu_context *ctx) // Jump relative function
{
    char rel = (char)(ctx->fetch_data & 0xFF);
    u16 address = ctx->regs.pc + rel;
    goto_address(ctx, address, false);
}

static void proc_call(cpu_context *ctx) // Call function, does push the pc
{
    goto_address(ctx, ctx->fetch_data, true);
}

static void proc_rst(cpu_context *ctx) // Restart function
{
    goto_address(ctx, ctx->cur_instruct->param, true);
}

static void proc_ret(cpu_context *ctx) // Return call function
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
        [IN_DI] = proc_di,
        [IN_XOR] = proc_xor};

IN_PROCESS inst_get_processor(instruction_type type)
{
    return processors[type];
}