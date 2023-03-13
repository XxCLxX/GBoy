#pragma once
#include <cpu.h>

//Interrupt Controller
//Interupt Flags
typedef enum
{
    IF_Joypad = 16,  //0x60
    IF_Serial = 8,   //0x58
    IF_Timer = 4,    //0x50
    IF_LCD_STAT = 2, //0x48
    IF_VBlank = 1    //0x40
}interrupt_type;

void request_interrupt(interrupt_type t);

void cpu_interrupt_handler(cpu_context *ctx);