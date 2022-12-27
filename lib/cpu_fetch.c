#include <cpu.h>
#include <bus.h>
#include <gboy.h>

static cpu_context ctx;

void fetched_data()
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

    //Fetch data from the 1st register
    case AM_R:
        ctx.fetch_data = register_read(ctx.cur_instruct->reg_1);
        return;

    //Fetch data from the 2nd register
    case AM_R_R:
        ctx.fetch_data = register_read(ctx.cur_instruct->reg_2);
        return;

    case AM_R_D8:
        ctx.fetch_data = bus_read(ctx.regs.pc);
        gboy_cycles(1);
        ctx.regs.pc++;
        return;

    case AM_R_D16: 
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

    //Loading register into memory region/address (Ex. Loading A to address BC)
    case AM_MR_R:
        ctx.fetch_data = register_read(ctx.cur_instruct->reg_2);
        ctx.memory_dest = register_read(ctx.cur_instruct->reg_1);
        ctx.dest_is_memory = true;

        if(ctx.cur_instruct->reg_1 == RT_C)
        {
            ctx.memory_dest |= 0xFF00;
        }
        return;

    //Reading from a memory region/address
    case AM_R_MR:
    {
        u16 address = register_read(ctx.cur_instruct->reg_2);

        if(ctx.cur_instruct->reg_2 == RT_C)
        {
            address |= 0xFF00;
        }

        ctx.fetch_data = bus_read(address);
        gboy_cycles(1);
    }return;

    case AM_R_HLI:
        ctx.fetch_data = bus_read(register_read(ctx.cur_instruct->reg_2));
        gboy_cycles(1);
        register_set(RT_HL, register_read(RT_HL) + 1);
        return;

    case AM_HLI_R:
        ctx.fetch_data = register_read(ctx.cur_instruct->reg_2);
        ctx.memory_dest = register_read(ctx.cur_instruct->reg_1);
        ctx.dest_is_memory = true;
        register_set(RT_HL, register_read(RT_HL) + 1);
        return;

    case AM_R_HLD:
        ctx.fetch_data = bus_read(register_read(ctx.cur_instruct->reg_2));
        gboy_cycles(1);
        register_set(RT_HL, register_read(RT_HL) - 1);
        return;

    case AM_HLD_R:
        ctx.fetch_data = register_read(ctx.cur_instruct->reg_2);
        ctx.memory_dest = register_read(ctx.cur_instruct->reg_1);
        ctx.dest_is_memory = true;
        register_set(RT_HL, register_read(RT_HL) - 1);
        return;

    case AM_R_A8:
        ctx.fetch_data = bus_read(ctx.regs.pc);
        gboy_cycles(1);
        ctx.regs.pc++;
        return;

    //Moving register to A8
    case AM_A8_R:
        ctx.memory_dest = bus_read(ctx.regs.pc) | 0xFF00;
        ctx.dest_is_memory = true;
        gboy_cycles(1);
        ctx.regs.pc++;
        return;

    //Load stack pointer to HL and incremented by r8 (only 1 instruction for this)
    case AM_HL_SPR:
        ctx.fetch_data = bus_read(ctx.regs.pc);
        gboy_cycles(1);
        ctx.regs.pc++;
        return;

    case AM_D8:
        ctx.fetch_data = bus_read(ctx.regs.pc);
        gboy_cycles(1);
        ctx.regs.pc++;
        return;

    case AM_A16_R:
    case AM_D16_R:
    {
        u16 low = bus_read(ctx.regs.pc);
        gboy_cycles(1);

        u16 high = bus_read(ctx.regs.pc+1);
        gboy_cycles(1);

        ctx.memory_dest = low | (high << 8);
        ctx.dest_is_memory = true;

        ctx.regs.pc += 2;
        ctx.fetch_data = register_read(ctx.cur_instruct->reg_2);
    }return;

    //Loading D8 to memory region/address
    case AM_MR_D8:
        ctx.fetch_data = bus_read(ctx.regs.pc);
        gboy_cycles(1);
        ctx.regs.pc++;
        ctx.memory_dest = register_read(ctx.cur_instruct->reg_1);
        ctx.dest_is_memory = true;
        return;

    case AM_MR:
        ctx.memory_dest = register_read(ctx.cur_instruct->reg_1);
        ctx.dest_is_memory = true;
        ctx.fetch_data = bus_read(register_read(ctx.cur_instruct->reg_1));
        gboy_cycles(1);
        return;

    case AM_R_A16: 
    {
        u16 low = bus_read(ctx.regs.pc);
        gboy_cycles(1);

        u16 high = bus_read(ctx.regs.pc+1);
        gboy_cycles(1);

        u16 address = low | (high << 8);
        ctx.regs.pc += 2;
        ctx.fetch_data = bus_read(address);
        gboy_cycles(1);

        return;
    }

    default:
        printf("Unknown Address Mode %d (%02X)\n", ctx.cur_instruct->mode, ctx.cur_opcode);
        exit(-7);
        return;
    }
}