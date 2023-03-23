#include <cpu.h>
#include <bus.h>
#include <gboy.h>
#include <interrupt.h>
#include <debugger.h>
#include <timer.h>

#define CPU_DEBUG 0 

cpu_context ctx = {0};

void cpu_init()
{
    ctx.regs.pc = 0x100;
    ctx.regs.sp = 0xFFFE;
    *((short *)&ctx.regs.a) = 0xB001;
    *((short *)&ctx.regs.b) = 0x1300;
    *((short *)&ctx.regs.d) = 0xD800;
    *((short *)&ctx.regs.h) = 0x4D01;
    ctx.ie_register = 0;
    ctx.interrupt_flag = 0;
    ctx.master_interrupt_enabled = false;
    ctx.enabling_mie = false;

    get_timer_ctx()->div = 0xABCC;
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

#if CPU_DEBUG == 1
        char flags[16];
        sprintf(flags, "%c%c%c%c",
                ctx.regs.f & (1 << 7) ? 'Z' : '-',
                ctx.regs.f & (1 << 6) ? 'N' : '-',
                ctx.regs.f & (1 << 5) ? 'H' : '-',
                ctx.regs.f & (1 << 4) ? 'C' : '-');

        char opcode[16];
        disassemble(&ctx, opcode);

        printf("%08lX - %04X: %-12s (%02X %02X %02X) A: %02X F: %s BC: %02X%02X DE: %02X%02X HL: %02X%02X \n", gboy_get_context()->ticks,
               pc, opcode, ctx.cur_opcode,
               bus_read(pc + 1), bus_read(pc + 2), ctx.regs.a, flags, ctx.regs.b, ctx.regs.c, ctx.regs.d, ctx.regs.e, ctx.regs.h, ctx.regs.l);
#endif
        if (ctx.cur_instruct == NULL)
        {
            printf("Unknown Instruction %02X\n", ctx.cur_opcode);
            exit(-7);
        }
        debugger_update();
        debugger_print();

        execute();
    }
    else
    {
        gboy_cycles(1);

        if (ctx.interrupt_flag)
        {
            ctx.halt = false;
        }
    }

    if (ctx.master_interrupt_enabled)
    {
        cpu_interrupt_handler(&ctx);
        ctx.enabling_mie = false;
    }

    if (ctx.enabling_mie)
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

void request_interrupt(interrupt_type t)
{
    ctx.interrupt_flag |= t;
}