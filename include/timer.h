#pragma once
#include <common.h>
// https://gbdev.io/pandocs/Timer_and_Divider_Registers.html

typedef struct
{
    u16 div; // Divder register
    u8 tima; // Time counter
    u8 tma;  // Timer modulo
    u8 tac;  // Timer control
} timer_context;

void timer_init();
void timer_tick();

u8 timer_read(u16 address);
void timer_write(u16 address, u8 value);

timer_context *get_timer_ctx();
