#include <cpu.h>
#include <gboy.h>

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

static void proc_ld(cpu_context *ctx) // Load function
{
}

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

static void proc_jp(cpu_context *ctx) // Jump function
{
    if (check_condition(ctx))
    {
        ctx->regs.pc = ctx->fetch_data;
        gboy_cycles(1);
    }
}

static IN_PROCESS processors[] =
    {
        [IN_NONE] = proc_none,
        [IN_NOP] = proc_nop,
        [IN_LD] = proc_ld,
        [IN_JP] = proc_jp,
        [IN_DI] = proc_di,
        [IN_XOR] = proc_xor};

IN_PROCESS inst_get_processor(instruction_type type)
{
    return processors[type];
}