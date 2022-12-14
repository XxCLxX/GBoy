#include <cpu.h>
#include <cpu_instruct.h>

instruction instructions[0x100] =
    {
        [0x00] = {IN_NOP, AM_IMP},

        [0x05] = {IN_DEC, AM_R, RT_B},

        [0X0E] = {IN_LD, AM_R_D8, RT_C},

        [0xAF] = {IN_XOR, AM_R, RT_A},

        [0xC3] = {IN_JP, AM_D16}};

instruction *instruction_opcode(u8 opcode)
{
    if (instructions[opcode].type == IN_NONE)
    {
        return NULL;
    }
    return &instructions[opcode]; // address of the opcode
}