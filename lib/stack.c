#include <stack.h>
#include <cpu.h>
#include <bus.h>

// PUSH - Decrement stack pointer
void stack_push(u8 data)
{
    get_register()->sp--;
    bus_write(get_register()->sp, data);
}

void stack_push16(u16 data)
{
    stack_push((data >> 8) & 0xFF);
    stack_push(data & 0xFF); 
}

//POP - Increment stack pointer
u8 stack_pop()
{
    return bus_read(get_register()->sp++);
}

u16 stack_pop16()
{
    u16 low = stack_pop();
    u16 high = stack_pop();

    return (high << 8 ) | low;
}