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

static void fetched_data()
{
    ctx.memory_dest = 0;
    ctx.dest_is_memory = false;

    if(ctx.cur_instruct == NULL)
    {
        return;
    }

    switch (ctx.cur_instruct->mode)
    {
    case AM_IMP:
        return;

    case AM_R:
        ctx.fetch_data = register_read(ctx.cur_instruct->reg_1);
        return;

    case AM_R_D8:
        ctx.fetch_data = bus_read(ctx.regs.pc);
        gboy_cycles(1);
        ctx.regs.pc++;
        return;

    case AM_D16:
    {
        u16 low = bus_read(ctx.regs.pc);
        gboy_cycles(1);

        u16 high = bus_read(ctx.regs.pc + 1);
        gboy_cycles(1);

        ctx.fetch_data = low | (high << 8);
        ctx.regs.pc += 2;
        return;
    }
    default:
        printf("Unknown Address Mode %d (%02X)\n", ctx.cur_instruct->mode, ctx.cur_opcode);
        exit(-7);
        return;
    }
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
