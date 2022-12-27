#include <cpu.h>
extern cpu_context ctx;

u16 reverse(u16 n)
{
    return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
}

u16 register_read(register_type rt)
{
    switch (rt)
    {

    case RT_A:
        return ctx.regs.a;
    case RT_F:
        return ctx.regs.f;
    case RT_B:
        return ctx.regs.b;
    case RT_C:
        return ctx.regs.c;
    case RT_D:
        return ctx.regs.d;
    case RT_E:
        return ctx.regs.e;
    case RT_H:
        return ctx.regs.h;
    case RT_L:
        return ctx.regs.l;

    case RT_AF:
        return reverse(*((u16 *)&ctx.regs.a));
    case RT_BC:
        return reverse(*((u16 *)&ctx.regs.b));
    case RT_DE:
        return reverse(*((u16 *)&ctx.regs.d));
    case RT_HL:
        return reverse(*((u16 *)&ctx.regs.h));

    case RT_PC:
        return ctx.regs.pc;
    case RT_SP:
        return ctx.regs.sp;
    default:
        return 0;
    }
}

void register_set(register_type rt, u16 n)
{
    switch (rt)
    {
    case RT_A:
        ctx.regs.a = n & 0xFF;
        break;
    case RT_F:
        ctx.regs.f = n & 0xFF;
        break;
    case RT_B:
        ctx.regs.b = n & 0xFF;
        break;
    case RT_C:
    {
        ctx.regs.c = n & 0xff;
    }
    break;
    case RT_D:
        ctx.regs.d = n & 0xFF;
        break;
    case RT_E:
        ctx.regs.e = n & 0xFF;
        break;
    case RT_H:
        ctx.regs.h = n & 0xFF;
        break;
    case RT_L:
        ctx.regs.l = n & 0xFF;
        break;

    case RT_AF:
        *((u16 *)&ctx.regs.a) = reverse(n);
        break;
    case RT_BC:
        *((u16 *)&ctx.regs.b) = reverse(n);
        break;
    case RT_DE:
        *((u16 *)&ctx.regs.d) = reverse(n);
        break;
    case RT_HL:
    {
        *((u16 *)&ctx.regs.h) = reverse(n);
        break;
    }
    case RT_PC:
        ctx.regs.pc = n;
        break;
    case RT_SP:
        ctx.regs.sp = n;
        break;
    case RT_NONE:
        break;
    }
}