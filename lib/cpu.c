#include <cpu.h>
#include <bus.h>
#include <gboy.h>

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
        fetched_data();

        printf("%04X: %-7s (%02X %02X %02X) A: %02X B: %02X C: %02X\n", 
        pc, instruction_name(ctx.cur_instruct->type), ctx.cur_opcode,
        bus_read(pc + 1), bus_read(pc + 2), ctx.regs.a, ctx.regs.b, ctx.regs.c);

        if (ctx.cur_instruct == NULL)
        {
            printf("Unknown Instruction %02X\n", ctx.cur_opcode);
            exit(-7);
        }
        execute();
    }
    // return false;
    return true;
}
