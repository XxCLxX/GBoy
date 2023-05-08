#include <cpu.h>
#include <cpu_instruct.h>
#include <bus.h>

instruction instructions[0x100] =
    {
        // 0x0X
        [0x00] = {OP_NOP, AM_IMP},
        [0x01] = {OP_LD, AM_REG_D16, RT_BC},
        [0x02] = {OP_LD, AM_MEMORYREG_REG, RT_BC, RT_A},
        [0x03] = {OP_INC, AM_REG, RT_BC},
        [0x04] = {OP_INC, AM_REG, RT_B},
        [0x05] = {OP_DEC, AM_REG, RT_B},
        [0x06] = {OP_LD, AM_REG_D8, RT_B},
        [0x07] = {OP_RLCA},
        [0x08] = {OP_LD, AM_A16_REG, RT_NONE, RT_SP},
        [0x09] = {OP_ADD, AM_REG_REG, RT_HL, RT_BC},
        [0x0A] = {OP_LD, AM_REG_MEMORYREG, RT_A, RT_BC},
        [0x0B] = {OP_DEC, AM_REG, RT_BC},
        [0x0C] = {OP_INC, AM_REG, RT_C},
        [0x0D] = {OP_DEC, AM_REG, RT_C},
        [0x0E] = {OP_LD, AM_REG_D8, RT_C},
        [0x0F] = {OP_RRCA},

        // 0x1X
        [0x10] = {OP_STOP},
        [0x11] = {OP_LD, AM_REG_D16, RT_DE},
        [0x12] = {OP_LD, AM_MEMORYREG_REG, RT_DE, RT_A},
        [0x13] = {OP_INC, AM_REG, RT_DE},
        [0x14] = {OP_INC, AM_REG, RT_D},
        [0x15] = {OP_DEC, AM_REG, RT_D},
        [0x16] = {OP_LD, AM_REG_D8, RT_D},
        [0x17] = {OP_RLA},
        [0x18] = {OP_JR, AM_D8},
        [0x19] = {OP_ADD, AM_REG_REG, RT_HL, RT_DE},
        [0x1A] = {OP_LD, AM_REG_MEMORYREG, RT_A, RT_DE},
        [0x1B] = {OP_DEC, AM_REG, RT_DE},
        [0x1C] = {OP_INC, AM_REG, RT_E},
        [0x1D] = {OP_DEC, AM_REG, RT_E},
        [0x1E] = {OP_LD, AM_REG_D8, RT_E},
        [0x1F] = {OP_RRA},

        // 0x2X
        [0x20] = {OP_JR, AM_D8, RT_NONE, RT_NONE, CT_NZ},
        [0x21] = {OP_LD, AM_REG_D16, RT_HL},
        [0x22] = {OP_LD, AM_HLI_REG, RT_HL, RT_A},
        [0x23] = {OP_INC, AM_REG, RT_HL},
        [0x24] = {OP_INC, AM_REG, RT_H},
        [0x25] = {OP_DEC, AM_REG, RT_H},
        [0x26] = {OP_LD, AM_REG_D8, RT_H},
        [0x27] = {OP_DAA},
        [0x28] = {OP_JR, AM_D8, RT_NONE, RT_NONE, CT_Z},
        [0x29] = {OP_ADD, AM_REG_REG, RT_HL, RT_HL},
        [0x2A] = {OP_LD, AM_REG_HLI, RT_A, RT_HL},
        [0x2B] = {OP_DEC, AM_REG, RT_HL},
        [0x2C] = {OP_INC, AM_REG, RT_L},
        [0x2D] = {OP_DEC, AM_REG, RT_L},
        [0x2E] = {OP_LD, AM_REG_D8, RT_L},
        [0x2F] = {OP_CPL},

        // 0x3X
        [0x30] = {OP_JR, AM_D8, RT_NONE, RT_NONE, CT_NC},
        [0x31] = {OP_LD, AM_REG_D16, RT_SP},
        [0x32] = {OP_LD, AM_HLD_REG, RT_HL, RT_A},
        [0x33] = {OP_INC, AM_REG, RT_SP},
        [0x34] = {OP_INC, AM_MEMORYREG, RT_HL},
        [0x35] = {OP_DEC, AM_MEMORYREG, RT_HL},
        [0x36] = {OP_LD, AM_MEMORYREG_D8, RT_HL},
        [0x37] = {OP_SCF},
        [0x38] = {OP_JR, AM_D8, RT_NONE, RT_NONE, CT_C},
        [0x39] = {OP_ADD, AM_REG_REG, RT_HL, RT_SP},
        [0x3A] = {OP_LD, AM_REG_HLD, RT_A, RT_HL},
        [0x3B] = {OP_DEC, AM_REG, RT_SP},
        [0x3C] = {OP_INC, AM_REG, RT_A},
        [0x3D] = {OP_DEC, AM_REG, RT_A},
        [0x3E] = {OP_LD, AM_REG_D8, RT_A},
        [0x3F] = {OP_CCF},

        // 0x4X
        [0x40] = {OP_LD, AM_REG_REG, RT_B, RT_B},
        [0x41] = {OP_LD, AM_REG_REG, RT_B, RT_C},
        [0x42] = {OP_LD, AM_REG_REG, RT_B, RT_D},
        [0x43] = {OP_LD, AM_REG_REG, RT_B, RT_E},
        [0x44] = {OP_LD, AM_REG_REG, RT_B, RT_H},
        [0x45] = {OP_LD, AM_REG_REG, RT_B, RT_L},
        [0x46] = {OP_LD, AM_REG_MEMORYREG, RT_B, RT_HL},
        [0x47] = {OP_LD, AM_REG_REG, RT_B, RT_A},
        [0x48] = {OP_LD, AM_REG_REG, RT_C, RT_B},
        [0x49] = {OP_LD, AM_REG_REG, RT_C, RT_C},
        [0x4A] = {OP_LD, AM_REG_REG, RT_C, RT_D},
        [0x4B] = {OP_LD, AM_REG_REG, RT_C, RT_E},
        [0x4C] = {OP_LD, AM_REG_REG, RT_C, RT_H},
        [0x4D] = {OP_LD, AM_REG_REG, RT_C, RT_L},
        [0x4E] = {OP_LD, AM_REG_MEMORYREG, RT_C, RT_HL},
        [0x4F] = {OP_LD, AM_REG_REG, RT_C, RT_A},

        // 0x5X
        [0x50] = {OP_LD, AM_REG_REG, RT_D, RT_B},
        [0x51] = {OP_LD, AM_REG_REG, RT_D, RT_C},
        [0x52] = {OP_LD, AM_REG_REG, RT_D, RT_D},
        [0x53] = {OP_LD, AM_REG_REG, RT_D, RT_E},
        [0x54] = {OP_LD, AM_REG_REG, RT_D, RT_H},
        [0x55] = {OP_LD, AM_REG_REG, RT_D, RT_L},
        [0x56] = {OP_LD, AM_REG_MEMORYREG, RT_D, RT_HL},
        [0x57] = {OP_LD, AM_REG_REG, RT_D, RT_A},
        [0x58] = {OP_LD, AM_REG_REG, RT_E, RT_B},
        [0x59] = {OP_LD, AM_REG_REG, RT_E, RT_C},
        [0x5A] = {OP_LD, AM_REG_REG, RT_E, RT_D},
        [0x5B] = {OP_LD, AM_REG_REG, RT_E, RT_E},
        [0x5C] = {OP_LD, AM_REG_REG, RT_E, RT_H},
        [0x5D] = {OP_LD, AM_REG_REG, RT_E, RT_L},
        [0x5E] = {OP_LD, AM_REG_MEMORYREG, RT_E, RT_HL},
        [0x5F] = {OP_LD, AM_REG_REG, RT_E, RT_A},

        // 0x6X
        [0x60] = {OP_LD, AM_REG_REG, RT_H, RT_B},
        [0x61] = {OP_LD, AM_REG_REG, RT_H, RT_C},
        [0x62] = {OP_LD, AM_REG_REG, RT_H, RT_D},
        [0x63] = {OP_LD, AM_REG_REG, RT_H, RT_E},
        [0x64] = {OP_LD, AM_REG_REG, RT_H, RT_H},
        [0x65] = {OP_LD, AM_REG_REG, RT_H, RT_L},
        [0x66] = {OP_LD, AM_REG_MEMORYREG, RT_H, RT_HL},
        [0x67] = {OP_LD, AM_REG_REG, RT_H, RT_A},
        [0x68] = {OP_LD, AM_REG_REG, RT_L, RT_B},
        [0x69] = {OP_LD, AM_REG_REG, RT_L, RT_C},
        [0x6A] = {OP_LD, AM_REG_REG, RT_L, RT_D},
        [0x6B] = {OP_LD, AM_REG_REG, RT_L, RT_E},
        [0x6C] = {OP_LD, AM_REG_REG, RT_L, RT_H},
        [0x6D] = {OP_LD, AM_REG_REG, RT_L, RT_L},
        [0x6E] = {OP_LD, AM_REG_MEMORYREG, RT_L, RT_HL},
        [0x6F] = {OP_LD, AM_REG_REG, RT_L, RT_A},

        // 0x7X
        [0x70] = {OP_LD, AM_MEMORYREG_REG, RT_HL, RT_B},
        [0x71] = {OP_LD, AM_MEMORYREG_REG, RT_HL, RT_C},
        [0x72] = {OP_LD, AM_MEMORYREG_REG, RT_HL, RT_D},
        [0x73] = {OP_LD, AM_MEMORYREG_REG, RT_HL, RT_E},
        [0x74] = {OP_LD, AM_MEMORYREG_REG, RT_HL, RT_H},
        [0x75] = {OP_LD, AM_MEMORYREG_REG, RT_HL, RT_L},
        [0x76] = {OP_HALT},
        [0x77] = {OP_LD, AM_MEMORYREG_REG, RT_HL, RT_A},
        [0x78] = {OP_LD, AM_REG_REG, RT_A, RT_B},
        [0x79] = {OP_LD, AM_REG_REG, RT_A, RT_C},
        [0x7A] = {OP_LD, AM_REG_REG, RT_A, RT_D},
        [0x7B] = {OP_LD, AM_REG_REG, RT_A, RT_E},
        [0x7C] = {OP_LD, AM_REG_REG, RT_A, RT_H},
        [0x7D] = {OP_LD, AM_REG_REG, RT_A, RT_L},
        [0x7E] = {OP_LD, AM_REG_MEMORYREG, RT_A, RT_HL},
        [0x7F] = {OP_LD, AM_REG_REG, RT_A, RT_A},

        // 0x8X
        [0x80] = {OP_ADD, AM_REG_REG, RT_A, RT_B},
        [0x81] = {OP_ADD, AM_REG_REG, RT_A, RT_C},
        [0x82] = {OP_ADD, AM_REG_REG, RT_A, RT_D},
        [0x83] = {OP_ADD, AM_REG_REG, RT_A, RT_E},
        [0x84] = {OP_ADD, AM_REG_REG, RT_A, RT_H},
        [0x85] = {OP_ADD, AM_REG_REG, RT_A, RT_L},
        [0x86] = {OP_ADD, AM_REG_MEMORYREG, RT_A, RT_HL},
        [0x87] = {OP_ADD, AM_REG_REG, RT_A, RT_A},
        [0x88] = {OP_ADC, AM_REG_REG, RT_A, RT_B},
        [0x89] = {OP_ADC, AM_REG_REG, RT_A, RT_C},
        [0x8A] = {OP_ADC, AM_REG_REG, RT_A, RT_D},
        [0x8B] = {OP_ADC, AM_REG_REG, RT_A, RT_E},
        [0x8C] = {OP_ADC, AM_REG_REG, RT_A, RT_H},
        [0x8D] = {OP_ADC, AM_REG_REG, RT_A, RT_L},
        [0x8E] = {OP_ADC, AM_REG_MEMORYREG, RT_A, RT_HL},
        [0x8F] = {OP_ADC, AM_REG_REG, RT_A, RT_A},

        // 0x9X
        [0x90] = {OP_SUB, AM_REG_REG, RT_A, RT_B},
        [0x91] = {OP_SUB, AM_REG_REG, RT_A, RT_C},
        [0x92] = {OP_SUB, AM_REG_REG, RT_A, RT_D},
        [0x93] = {OP_SUB, AM_REG_REG, RT_A, RT_E},
        [0x94] = {OP_SUB, AM_REG_REG, RT_A, RT_H},
        [0x95] = {OP_SUB, AM_REG_REG, RT_A, RT_L},
        [0x96] = {OP_SUB, AM_REG_MEMORYREG, RT_A, RT_HL},
        [0x97] = {OP_SUB, AM_REG_REG, RT_A, RT_A},
        [0x98] = {OP_SBC, AM_REG_REG, RT_A, RT_B},
        [0x99] = {OP_SBC, AM_REG_REG, RT_A, RT_C},
        [0x9A] = {OP_SBC, AM_REG_REG, RT_A, RT_D},
        [0x9B] = {OP_SBC, AM_REG_REG, RT_A, RT_E},
        [0x9C] = {OP_SBC, AM_REG_REG, RT_A, RT_H},
        [0x9D] = {OP_SBC, AM_REG_REG, RT_A, RT_L},
        [0x9E] = {OP_SBC, AM_REG_MEMORYREG, RT_A, RT_HL},
        [0x9F] = {OP_SBC, AM_REG_REG, RT_A, RT_A},

        // 0xAX
        [0xA0] = {OP_AND, AM_REG_REG, RT_A, RT_B},
        [0xA1] = {OP_AND, AM_REG_REG, RT_A, RT_C},
        [0xA2] = {OP_AND, AM_REG_REG, RT_A, RT_D},
        [0xA3] = {OP_AND, AM_REG_REG, RT_A, RT_E},
        [0xA4] = {OP_AND, AM_REG_REG, RT_A, RT_H},
        [0xA5] = {OP_AND, AM_REG_REG, RT_A, RT_L},
        [0xA6] = {OP_AND, AM_REG_MEMORYREG, RT_A, RT_HL},
        [0xA7] = {OP_AND, AM_REG_REG, RT_A, RT_A},
        [0xA8] = {OP_XOR, AM_REG_REG, RT_A, RT_B},
        [0xA9] = {OP_XOR, AM_REG_REG, RT_A, RT_C},
        [0xAA] = {OP_XOR, AM_REG_REG, RT_A, RT_D},
        [0xAB] = {OP_XOR, AM_REG_REG, RT_A, RT_E},
        [0xAC] = {OP_XOR, AM_REG_REG, RT_A, RT_H},
        [0xAD] = {OP_XOR, AM_REG_REG, RT_A, RT_L},
        [0xAE] = {OP_XOR, AM_REG_MEMORYREG, RT_A, RT_HL},
        [0xAF] = {OP_XOR, AM_REG_REG, RT_A, RT_A},

        // 0xBX
        [0xB0] = {OP_OR, AM_REG_REG, RT_A, RT_B},
        [0xB1] = {OP_OR, AM_REG_REG, RT_A, RT_C},
        [0xB2] = {OP_OR, AM_REG_REG, RT_A, RT_D},
        [0xB3] = {OP_OR, AM_REG_REG, RT_A, RT_E},
        [0xB4] = {OP_OR, AM_REG_REG, RT_A, RT_H},
        [0xB5] = {OP_OR, AM_REG_REG, RT_A, RT_L},
        [0xB6] = {OP_OR, AM_REG_MEMORYREG, RT_A, RT_HL},
        [0xB7] = {OP_OR, AM_REG_REG, RT_A, RT_A},
        [0xB8] = {OP_CP, AM_REG_REG, RT_A, RT_B},
        [0xB9] = {OP_CP, AM_REG_REG, RT_A, RT_C},
        [0xBA] = {OP_CP, AM_REG_REG, RT_A, RT_D},
        [0xBB] = {OP_CP, AM_REG_REG, RT_A, RT_E},
        [0xBC] = {OP_CP, AM_REG_REG, RT_A, RT_H},
        [0xBD] = {OP_CP, AM_REG_REG, RT_A, RT_L},
        [0xBE] = {OP_CP, AM_REG_MEMORYREG, RT_A, RT_HL},
        [0xBF] = {OP_CP, AM_REG_REG, RT_A, RT_A},

        // 0xCX
        [0xC0] = {OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_NZ},
        [0xC1] = {OP_POP, AM_REG, RT_BC},
        [0xC2] = {OP_JP, AM_D16, RT_NONE, RT_NONE, CT_NZ},
        [0xC3] = {OP_JP, AM_D16},
        [0xC4] = {OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_NZ},
        [0xC5] = {OP_PUSH, AM_REG, RT_BC},
        [0xC6] = {OP_ADD, AM_REG_D8, RT_A},
        [0xC7] = {OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00},
        [0xC8] = {OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_Z},
        [0xC9] = {OP_RET},
        [0xCA] = {OP_JP, AM_D16, RT_NONE, RT_NONE, CT_Z},
        [0xCB] = {OP_CB, AM_D8},
        [0xCC] = {OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_Z},
        [0xCD] = {OP_CALL, AM_D16},
        [0xCE] = {OP_ADC, AM_REG_D8, RT_A},
        [0xCF] = {OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x08},

        // 0xDX
        [0xD0] = {OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_NC},
        [0xD1] = {OP_POP, AM_REG, RT_DE},
        [0xD2] = {OP_JP, AM_D16, RT_NONE, RT_NONE, CT_NC},
        [0xD4] = {OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_NC},
        [0xD5] = {OP_PUSH, AM_REG, RT_DE},
        [0xD6] = {OP_SUB, AM_REG_D8, RT_A},
        [0xD7] = {OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x10},
        [0xD8] = {OP_RET, AM_IMP, RT_NONE, RT_NONE, CT_C},
        [0xD9] = {OP_RETI},
        [0xDA] = {OP_JP, AM_D16, RT_NONE, RT_NONE, CT_C},
        [0xDC] = {OP_CALL, AM_D16, RT_NONE, RT_NONE, CT_C},
        [0xDE] = {OP_SBC, AM_REG_D8, RT_A},
        [0xDF] = {OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x18},

        // 0xEX
        [0xE0] = {OP_LDH, AM_A8_REG, RT_NONE, RT_A},
        [0xE1] = {OP_POP, AM_REG, RT_HL},
        [0xE2] = {OP_LD, AM_MEMORYREG_REG, RT_C, RT_A},
        [0xE5] = {OP_PUSH, AM_REG, RT_HL},
        [0xE6] = {OP_AND, AM_REG_D8, RT_A},
        [0xE7] = {OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x20},
        [0xE8] = {OP_ADD, AM_REG_D8, RT_SP},
        [0xE9] = {OP_JP, AM_REG, RT_HL},
        [0xEA] = {OP_LD, AM_A16_REG, RT_NONE, RT_A},
        [0xEE] = {OP_XOR, AM_REG_D8, RT_A},
        [0xEF] = {OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x28},

        // 0xFX
        [0xF0] = {OP_LDH, AM_REG_A8, RT_A},
        [0xF1] = {OP_POP, AM_REG, RT_AF},
        [0xF2] = {OP_LD, AM_REG_MEMORYREG, RT_A, RT_C},
        [0xF3] = {OP_DI},
        [0xF5] = {OP_PUSH, AM_REG, RT_AF},
        [0xF6] = {OP_OR, AM_REG_D8, RT_A},
        [0xF7] = {OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x30},
        [0xF8] = {OP_LD, AM_HL_SPR, RT_HL, RT_SP},
        [0xF9] = {OP_LD, AM_REG_REG, RT_SP, RT_HL},
        [0xFA] = {OP_LD, AM_REG_A16, RT_A},
        [0xFB] = {OP_EI},
        [0xFE] = {OP_CP, AM_REG_D8, RT_A},
        [0xFF] = {OP_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x38},
};

instruction *instruction_opcode(u8 opcode)
{
    return &instructions[opcode]; // address of the opcode
}
char *instruction_lookup[] =
    {
        "N/A",
        "NOP",
        "LD",
        "INC",
        "DEC",
        "RLCA",
        "ADD",
        "RRCA",
        "STOP",
        "RLA",
        "JR",
        "RRA",
        "DAA",
        "CPL",
        "SCF",
        "CCF",
        "HALT",
        "ADC",
        "SUB",
        "SBC",
        "AND",
        "XOR",
        "OR",
        "CP",
        "POP",
        "JP",
        "PUSH",
        "RET",
        "CB",
        "CALL",
        "RETI",
        "LDH",
        "JPHL",
        "DI",
        "EI",
        "RST",
        "OP_ERR",
        "OP_RLC",
        "OP_RRC",
        "OP_RL",
        "OP_RR",
        "OP_SLA",
        "OP_SRA",
        "OP_SWAP",
        "OP_SRL",
        "OP_BIT",
        "OP_RES",
        "OP_SET"};

char *instruction_name(instruction_type type)
{
    return instruction_lookup[type];
}

static char *register_lookup[] =
    {
        "N/A",
        "A",
        "F",
        "B",
        "C",
        "D",
        "E",
        "H",
        "L",
        "AF",
        "BC",
        "DE",
        "HL",
        "SP",
        "PC"};

// Convert an instruction to a disassembled format (instruction to string)
void disassemble(cpu_context *ctx, char *str)
{
    instruction *opcode = ctx->cur_instruct;
    sprintf(str, "%s ", instruction_name(opcode->type));

    switch (opcode->mode)
    {
    case AM_IMP:
        return;

    case AM_REG_D16:
    case AM_REG_A16:
        sprintf(str, "%s %s,$%04X", instruction_name(opcode->type),
                register_lookup[opcode->reg_1], ctx->fetch_data);
        return;

    case AM_REG:
        sprintf(str, "%s %s", instruction_name(opcode->type),
                register_lookup[opcode->reg_1]);
        return;

    case AM_REG_REG:
        sprintf(str, "%s %s,%s", instruction_name(opcode->type),
                register_lookup[opcode->reg_1], register_lookup[opcode->reg_2]);
        return;

    case AM_MEMORYREG_REG:
        sprintf(str, "%s (%s),%s", instruction_name(opcode->type),
                register_lookup[opcode->reg_1], register_lookup[opcode->reg_2]);
        return;

    case AM_MEMORYREG:
        sprintf(str, "%s (%s)", instruction_name(opcode->type),
                register_lookup[opcode->reg_1]);
        return;

    case AM_REG_MEMORYREG:
        sprintf(str, "%s %s,(%s)", instruction_name(opcode->type),
                register_lookup[opcode->reg_1], register_lookup[opcode->reg_2]);
        return;

    case AM_REG_D8:
    case AM_REG_A8:
        sprintf(str, "%s %s,$%02X", instruction_name(opcode->type),
                register_lookup[opcode->reg_1], ctx->fetch_data & 0xFF);
        return;

    case AM_REG_HLI:
        sprintf(str, "%s %s,(%s+)", instruction_name(opcode->type),
                register_lookup[opcode->reg_1], register_lookup[opcode->reg_2]);
        return;

    case AM_REG_HLD:
        sprintf(str, "%s %s,(%s-)", instruction_name(opcode->type),
                register_lookup[opcode->reg_1], register_lookup[opcode->reg_2]);
        return;

    case AM_HLI_REG:
        sprintf(str, "%s (%s+),%s", instruction_name(opcode->type),
                register_lookup[opcode->reg_1], register_lookup[opcode->reg_2]);
        return;

    case AM_HLD_REG:
        sprintf(str, "%s (%s-),%s", instruction_name(opcode->type),
                register_lookup[opcode->reg_1], register_lookup[opcode->reg_2]);
        return;

    case AM_A8_REG:
        sprintf(str, "%s $%02X,%s", instruction_name(opcode->type),
                bus_read(ctx->regs.pc - 1), register_lookup[opcode->reg_2]);

        return;

    case AM_HL_SPR:
        sprintf(str, "%s (%s),SP+%d", instruction_name(opcode->type),
                register_lookup[opcode->reg_1], ctx->fetch_data & 0xFF);
        return;

    case AM_D8:
        sprintf(str, "%s $%02X", instruction_name(opcode->type),
                ctx->fetch_data & 0xFF);
        return;

    case AM_D16:
        sprintf(str, "%s $%04X", instruction_name(opcode->type),
                ctx->fetch_data);
        return;

    case AM_MEMORYREG_D8:
        sprintf(str, "%s (%s),$%02X", instruction_name(opcode->type),
                register_lookup[opcode->reg_1], ctx->fetch_data & 0xFF);
        return;

    case AM_A16_REG:
        sprintf(str, "%s ($%04X),%s", instruction_name(opcode->type),
                ctx->fetch_data, register_lookup[opcode->reg_2]);
        return;

    default:
        fprintf(stderr, "Invalid Address Mode: %d\n", opcode->mode);
        NO_IMPLEM
    }
}
