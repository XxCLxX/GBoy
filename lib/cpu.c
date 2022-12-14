#include <cpu.h>
#include <bus.h>
#include <gboy.h>

cpu_context ctx = {0};

void cpu_init()
{
    ctx.regs.pc = 0x100;
}

static void fetch_instruction()
{
    ctx.cur_opcode = bus_read(ctx.regs.pc++);
    ctx.cur_instruct = instruction_opcode(ctx.cur_opcode);

    if (ctx.cur_instruct == NULL)
    {
        printf("Unknown Instruction %02X\n", ctx.cur_opcode);
        exit(-7);
    }
}

static void fetched_data()
{
    ctx.memory_dest = 0;
    ctx.dest_is_memory = false;

    switch (ctx.cur_instruct->mode)
    {
        case AM_IMP:
            return;

        case AM_R:
            ctx.fetch_data = register_read(ctx.regs.pc);
            gboy_cycles(1);
            ctx.regs.pc++;
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
            printf("Unknown Address Mdoe %d\n", ctx.cur_instruct->mode);
            exit(-7);
            return;
        }
}

static void execute()
{
    printf("\tNot executing...\n");
}

bool cpu_run()
{
    if (!ctx.halt)
    {
        u16 pc = ctx.regs.pc;

        fetch_instruction();
        fetched_data();
        printf("Executing Instructions: %02X    PC: %04X\n", ctx.cur_opcode, pc);
        execute();
    }
    // return false;
    return true;
}
