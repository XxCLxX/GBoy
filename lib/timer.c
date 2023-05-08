#include <timer.h>
#include <interrupt.h>
// https://gbdev.io/pandocs/Timer_and_Divider_Registers.html

static timer_context ctx = {0};

timer_context *get_timer_ctx()
{
    return &ctx;
}

void timer_init()
{
    ctx.div = 0xAC00;
}

void timer_tick()
{
    u16 prev_div = ctx.div;

    ctx.div++;

    bool timer_update = false;

    switch (ctx.tac & (0b11))
    {
    case 0b00:
        timer_update = (prev_div & (1 << 9)) && (!(ctx.div & (1 << 9)));
        break;

    case 0b01:
        timer_update = (prev_div & (1 << 3)) && (!(ctx.div & (1 << 3)));
        break;

    case 0b10:
        timer_update = (prev_div & (1 << 5)) && (!(ctx.div & (1 << 5)));
        break;

    case 0b11:
        timer_update = (prev_div & (1 << 7)) && (!(ctx.div & (1 << 7)));
        break;
    }

    if (timer_update && ctx.tac & (1 << 2))
    {
        ctx.tima++;

        if (ctx.tima == 0xFF)
        {
            ctx.tima = ctx.tma;
            request_interrupt(IF_Timer);
        }
    }
}

u8 timer_read(u16 address)
{
    switch (address)
    {
        // DIV: Divider register
        case 0xFF04:
            return ctx.div >> 8;

        // TIMA: Timer counter
        case 0xFF05:
            return ctx.tima;

        // TMA: Timer modulo
        case 0xFF06:
            return ctx.tma;

        // TAC: Timer control
        case 0xFF07:
            return ctx.tac;
    }
}

void timer_write(u16 address, u8 value)
{
    switch (address)
    {
        // DIV: Divider register
        case 0xFF04:
            ctx.div = 0;
            break;

        // TIMA: Timer counter
        case 0xFF05:
            ctx.tima = value;
            break;

        // TMA: Timer modulo
        case 0xFF06:
            ctx.tma = value;
            break;

        // TAC: Timer control
        case 0xFF07:
            ctx.tac = value;
            break;
    }
}
