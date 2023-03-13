#include <cpu.h>
#include <stack.h>
#include <interrupt.h>

void interrupt_handler(cpu_context *ctx, u16 address)
{
    stack_push16(ctx->regs.pc);
    ctx->regs.pc = address;
}

bool interrupt_check(cpu_context *ctx, u16 address, interrupt_type t)
{
    if(ctx->interrupt_flag & t && ctx->ie_register & t)
    {
        interrupt_handler(ctx, address);
        ctx->interrupt_flag &= ~t;
        ctx->halt = false;
        ctx->master_interrupt_enabled = false;

        return true;
    }
    return false;
}

//void request_interrupt(interrupt_type t);

void cpu_interrupt_handler(cpu_context *ctx)
{
    if(interrupt_check(ctx, 0x40, IF_VBlank))
    {

    }
    else if (interrupt_check(ctx, 0x48, IF_LCD_STAT))
    {
        
    }
    else if(interrupt_check(ctx, 0x50, IF_Timer))
    {

    }
    else if(interrupt_check(ctx, 0x58, IF_Serial))
    {

    }
    else if(interrupt_check(ctx, 0x60, IF_Joypad))
    {

    }
}