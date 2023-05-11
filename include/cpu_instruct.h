#pragma once
#include <common.h>

// Reference - GameBoy (LR35902) OPCODES
// Addressing mode
typedef enum
{
    AM_IMP,
    AM_REG_D16,
    AM_REG_REG,
    AM_MEMORYREG_REG,
    AM_REG,
    AM_REG_D8,
    AM_REG_MEMORYREG,
    AM_REG_HLI,
    AM_REG_HLD,
    AM_HLI_REG,
    AM_HLD_REG,
    AM_REG_A8,
    AM_A8_REG,
    AM_HL_SPR,
    AM_D16,
    AM_D8,
    AM_D16_REG,
    AM_MEMORYREG_D8,
    AM_MEMORYREG,
    AM_A16_REG,
    AM_REG_A16
} address_mode;

// Registers
typedef enum
{
    RT_NONE,
    RT_A,
    RT_F,
    RT_B,
    RT_C,
    RT_D,
    RT_E,
    RT_H,
    RT_L,
    RT_AF,
    RT_BC,
    RT_DE,
    RT_HL,
    RT_SP,
    RT_PC
} register_type;

// Instructions
typedef enum
{
    OP_NONE,
    OP_NOP,
    OP_LD,
    OP_INC,
    OP_DEC,
    OP_RLCA,
    OP_ADD,
    OP_RRCA,
    OP_STOP,
    OP_RLA,
    OP_JR,
    OP_RRA,
    OP_DAA,
    OP_CPL,
    OP_SCF,
    OP_CCF,
    OP_HALT,
    OP_ADC,
    OP_SUB,
    OP_SBC,
    OP_AND,
    OP_XOR,
    OP_OR,
    OP_CP,
    OP_POP,
    OP_JP,
    OP_PUSH,
    OP_RET,
    OP_CB,
    OP_CALL,
    OP_RETI,
    OP_LDH,
    OP_JPHL,
    OP_DI,
    OP_EI,
    OP_RST,
    OP_ERR,
    OP_RLC, // CB instructions
    OP_RRC,
    OP_RL,
    OP_RR,
    OP_SLA,
    OP_SRA,
    OP_SWAP,
    OP_SRL,
    OP_BIT,
    OP_RES,
    OP_SET
} instruction_type;

/*Flag condtions
Z - Zero Flag
N - Subtract Flag
H - Half Carry Flag
C - Carry Flag */
typedef enum
{
    CT_NONE,
    CT_NZ,
    CT_Z,
    CT_NC,
    CT_C
} condition_type;

typedef struct
{
    instruction_type type;
    address_mode mode;
    register_type reg_1;
    register_type reg_2;
    condition_type cond;
    u8 param;
} instruction;

instruction *instruction_opcode(u8 opcode);
char *instruction_name(instruction_type type);