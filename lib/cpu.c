#include <cpu.h>
#include <bus.h>
#include <gboy.h>
#include <interrupt.h>

cpu_context ctx = {0};

void cpu_init()
{
    ctx.regs.pc = 0x100;
    ctx.regs.a = 0x01;
}

static void fetch_instruction()
{
    ctx.cur_opcode = bus_read(ctx.regs.pc++);
    ctx.cur_instruct = instruction_opcode(ctx.cur_opcode);
}

void fetched_data();

static void execute()
{
    IN_PROCESS process = inst_get_processor(ctx.cur_instruct->type);

    if (!process)
    {
        NO_IMPLEM
    }

    process(&ctx);
}

bool cpu_run()
{
    if (!ctx.halt)
    {
        u16 pc = ctx.regs.pc;

        fetch_instruction();
        gboy_cycles(1);
        fetched_data();

        char flags[16];
        sprintf(flags, "%c%c%c%c",
                ctx.regs.f & (1 << 7) ? 'Z' : '-',
                ctx.regs.f & (1 << 6) ? 'N' : '-',
                ctx.regs.f & (1 << 5) ? 'H' : '-',
                ctx.regs.f & (1 << 4) ? 'C' : '-');

        printf("%08lX - %04X: %-7s (%02X %02X %02X) A: %02X F: %s BC: %02X%02X DE: %02X%02X HL: %02X%02X \n", gboy_get_context()->ticks,
               pc, instruction_name(ctx.cur_instruct->type), ctx.cur_opcode,
               bus_read(pc + 1), bus_read(pc + 2), ctx.regs.a, flags, ctx.regs.b, ctx.regs.c, ctx.regs.d, ctx.regs.e, ctx.regs.h, ctx.regs.l);

        if (ctx.cur_instruct == NULL)
        {
            printf("Unknown Instruction %02X\n", ctx.cur_opcode);
            exit(-7);
        }
        execute();
    } 
    else
    {
        gboy_cycles(1);

        if(ctx.interrupt_flag)
        {
            ctx.halt = false;
        }
    }

    if(ctx.master_interrupt_enabled)
    {
        cpu_interrupt_handler(&ctx);
        ctx.enabling_mie = false;
    }

    if(ctx.enabling_mie)
    {
        ctx.master_interrupt_enabled = true;
    }

    // return false;
    return true;
}

u8 ie_register_get()
{
    return ctx.ie_register;
}

void ie_register_set(u8 n)
{
    ctx.ie_register = n;
}
