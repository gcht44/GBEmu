#include "CPU.hpp"
#include "Stack.hpp"

/*
Status development:
    01 - special  -> OK
    03 - op sp,hl -> Test Unsupported addressing mode: 13
    06 - ld r,r   -> OK
*/

CPU::CPU() : stack()
{
    // reg = { 0 };
    // for gameboy doctor
	reg.A = 0x01; 
	reg.F = 0xB0; 
	reg.B = 0x00;
	reg.C = 0x13;
	reg.D = 0x00;
	reg.E = 0xD8;
	reg.H = 0x01;
	reg.L = 0x4D;
	reg.SP = 0xFFFE;

    reg.PC = 0x100;
	halted = false;
	running = true;
	destIsMem = false;

    msglgth = 0;

    OpcodeTable = {
        // 0x00 - 0x0F
        {0x00, {IN_NOP, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0x01, {IN_LD, AM_R_D16, RT_BC, RT_NONE, CT_NONE}},
        {0x02, {IN_LD, AM_MR_R, RT_BC, RT_A, CT_NONE}},
        {0x03, {IN_INC, AM_R, RT_BC, RT_NONE, CT_NONE}},
        {0x04, {IN_INC, AM_R, RT_B, RT_NONE, CT_NONE}},
        {0x05, {IN_DEC, AM_R, RT_B, RT_NONE, CT_NONE}},
        {0x06, {IN_LD, AM_R_D8, RT_B, RT_NONE, CT_NONE}},
        {0x07, {IN_RLCA, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0x08, {IN_LD, AM_A16_R, RT_NONE, RT_SP, CT_NONE}},
        {0x09, {IN_ADD, AM_R_R, RT_HL, RT_BC, CT_NONE}},
        {0x0A, {IN_LD, AM_R_MR, RT_A, RT_BC, CT_NONE}},
        {0x0B, {IN_DEC, AM_R, RT_BC, RT_NONE, CT_NONE}},
        {0x0C, {IN_INC, AM_R, RT_C, RT_NONE, CT_NONE}},
        {0x0D, {IN_DEC, AM_R, RT_C, RT_NONE, CT_NONE}},
        {0x0E, {IN_LD, AM_R_D8, RT_C, RT_NONE, CT_NONE}},
        {0x0F, {IN_RRCA, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},

        // 0x10 - 0x1F
        {0x10, {IN_STOP, AM_D8, RT_NONE, RT_NONE, CT_NONE}},
        {0x11, {IN_LD, AM_R_D16, RT_DE, RT_NONE, CT_NONE}},
        {0x12, {IN_LD, AM_MR_R, RT_DE, RT_A, CT_NONE}},
        {0x13, {IN_INC, AM_R, RT_DE, RT_NONE, CT_NONE}},
        {0x14, {IN_INC, AM_R, RT_D, RT_NONE, CT_NONE}},
        {0x15, {IN_DEC, AM_R, RT_D, RT_NONE, CT_NONE}},
        {0x16, {IN_LD, AM_R_D8, RT_D, RT_NONE, CT_NONE}},
        {0x17, {IN_RLA, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0x18, {IN_JR, AM_D8, RT_NONE, RT_NONE, CT_NONE}},
        {0x19, {IN_ADD, AM_R_R, RT_HL, RT_DE, CT_NONE}},
        {0x1A, {IN_LD, AM_R_MR, RT_A, RT_DE, CT_NONE}},
        {0x1B, {IN_DEC, AM_R, RT_DE, RT_NONE, CT_NONE}},
        {0x1C, {IN_INC, AM_R, RT_E, RT_NONE, CT_NONE}},
        {0x1D, {IN_DEC, AM_R, RT_E, RT_NONE, CT_NONE}},
        {0x1E, {IN_LD, AM_R_D8, RT_E, RT_NONE, CT_NONE}},
        {0x1F, {IN_RRA, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},

        // 0x20 - 0x2F
        {0x20, {IN_JR, AM_D8, RT_NONE, RT_NONE, CT_NZ}},
        {0x21, {IN_LD, AM_R_D16, RT_HL, RT_NONE, CT_NONE}},
        {0x22, {IN_LD, AM_HLI_R, RT_NONE, RT_A, CT_NONE}},
        {0x23, {IN_INC, AM_R, RT_HL, RT_NONE, CT_NONE}},
        {0x24, {IN_INC, AM_R, RT_H, RT_NONE, CT_NONE}},
        {0x25, {IN_DEC, AM_R, RT_H, RT_NONE, CT_NONE}},
        {0x26, {IN_LD, AM_R_D8, RT_H, RT_NONE, CT_NONE}},
        {0x27, {IN_DAA, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0x28, {IN_JR, AM_D8, RT_NONE, RT_NONE, CT_Z}},
        {0x29, {IN_ADD, AM_R_R, RT_HL, RT_HL, CT_NONE}},
        {0x2A, {IN_LD, AM_R_HLI, RT_A, RT_NONE, CT_NONE}},
        {0x2B, {IN_DEC, AM_R, RT_HL, RT_NONE, CT_NONE}},
        {0x2C, {IN_INC, AM_R, RT_L, RT_NONE, CT_NONE}},
        {0x2D, {IN_DEC, AM_R, RT_L, RT_NONE, CT_NONE}},
        {0x2E, {IN_LD, AM_R_D8, RT_L, RT_NONE, CT_NONE}},
        {0x2F, {IN_CPL, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},

        // 0x30 - 0x3F
        {0x30, {IN_JR, AM_D8, RT_NONE, RT_NONE, CT_NC}},
        {0x31, {IN_LD, AM_R_D16, RT_SP, RT_NONE, CT_NONE}},
        {0x32, {IN_LD, AM_HLD_R, RT_NONE, RT_A, CT_NONE}},
        {0x33, {IN_INC, AM_R, RT_SP, RT_NONE, CT_NONE}},
        {0x34, {IN_INC, AM_MR, RT_HL, RT_NONE, CT_NONE}},
        {0x35, {IN_DEC, AM_MR, RT_HL, RT_NONE, CT_NONE}},
        {0x36, {IN_LD, AM_MR_D8, RT_HL, RT_NONE, CT_NONE}},
        {0x37, {IN_SCF, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0x38, {IN_JR, AM_D8, RT_NONE, RT_NONE, CT_C}},
        {0x39, {IN_ADD, AM_R_R, RT_HL, RT_SP, CT_NONE}},
        {0x3A, {IN_LD, AM_R_HLD, RT_A, RT_NONE, CT_NONE}},
        {0x3B, {IN_DEC, AM_R, RT_SP, RT_NONE, CT_NONE}},
        {0x3C, {IN_INC, AM_R, RT_A, RT_NONE, CT_NONE}},
        {0x3D, {IN_DEC, AM_R, RT_A, RT_NONE, CT_NONE}},
        {0x3E, {IN_LD, AM_R_D8, RT_A, RT_NONE, CT_NONE}},
        {0x3F, {IN_CCF, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},

        // 0x40 - 0x4F
        {0x40, {IN_LD, AM_R_R, RT_B, RT_B, CT_NONE}},
        {0x41, {IN_LD, AM_R_R, RT_B, RT_C, CT_NONE}},
        {0x42, {IN_LD, AM_R_R, RT_B, RT_D, CT_NONE}},
        {0x43, {IN_LD, AM_R_R, RT_B, RT_E, CT_NONE}},
        {0x44, {IN_LD, AM_R_R, RT_B, RT_H, CT_NONE}},
        {0x45, {IN_LD, AM_R_R, RT_B, RT_L, CT_NONE}},
        {0x46, {IN_LD, AM_R_MR, RT_B, RT_HL, CT_NONE}},
        {0x47, {IN_LD, AM_R_R, RT_B, RT_A, CT_NONE}},
        {0x48, {IN_LD, AM_R_R, RT_C, RT_B, CT_NONE}},
        {0x49, {IN_LD, AM_R_R, RT_C, RT_C, CT_NONE}},
        {0x4A, {IN_LD, AM_R_R, RT_C, RT_D, CT_NONE}},
        {0x4B, {IN_LD, AM_R_R, RT_C, RT_E, CT_NONE}},
        {0x4C, {IN_LD, AM_R_R, RT_C, RT_H, CT_NONE}},
        {0x4D, {IN_LD, AM_R_R, RT_C, RT_L, CT_NONE}},
        {0x4E, {IN_LD, AM_R_MR, RT_C, RT_HL, CT_NONE}},
        {0x4F, {IN_LD, AM_R_R, RT_C, RT_A, CT_NONE}},

        // 0x50 - 0x5F
        {0x50, {IN_LD, AM_R_R, RT_D, RT_B, CT_NONE}},
        {0x51, {IN_LD, AM_R_R, RT_D, RT_C, CT_NONE}},
        {0x52, {IN_LD, AM_R_R, RT_D, RT_D, CT_NONE}},
        {0x53, {IN_LD, AM_R_R, RT_D, RT_E, CT_NONE}},
        {0x54, {IN_LD, AM_R_R, RT_D, RT_H, CT_NONE}},
        {0x55, {IN_LD, AM_R_R, RT_D, RT_L, CT_NONE}},
        {0x56, {IN_LD, AM_R_MR, RT_D, RT_HL, CT_NONE}},
        {0x57, {IN_LD, AM_R_R, RT_D, RT_A, CT_NONE}},
        {0x58, {IN_LD, AM_R_R, RT_E, RT_B, CT_NONE}},
        {0x59, {IN_LD, AM_R_R, RT_E, RT_C, CT_NONE}},
        {0x5A, {IN_LD, AM_R_R, RT_E, RT_D, CT_NONE}},
        {0x5B, {IN_LD, AM_R_R, RT_E, RT_E, CT_NONE}},
        {0x5C, {IN_LD, AM_R_R, RT_E, RT_H, CT_NONE}},
        {0x5D, {IN_LD, AM_R_R, RT_E, RT_L, CT_NONE}},
        {0x5E, {IN_LD, AM_R_MR, RT_E, RT_HL, CT_NONE}},
        {0x5F, {IN_LD, AM_R_R, RT_E, RT_A, CT_NONE}},

        // 0x60 - 0x6F
        {0x60, {IN_LD, AM_R_R, RT_H, RT_B, CT_NONE}},
        {0x61, {IN_LD, AM_R_R, RT_H, RT_C, CT_NONE}},
        {0x62, {IN_LD, AM_R_R, RT_H, RT_D, CT_NONE}},
        {0x63, {IN_LD, AM_R_R, RT_H, RT_E, CT_NONE}},
        {0x64, {IN_LD, AM_R_R, RT_H, RT_H, CT_NONE}},
        {0x65, {IN_LD, AM_R_R, RT_H, RT_L, CT_NONE}},
        {0x66, {IN_LD, AM_R_MR, RT_H, RT_HL, CT_NONE}},
        {0x67, {IN_LD, AM_R_R, RT_H, RT_A, CT_NONE}},
        {0x68, {IN_LD, AM_R_R, RT_L, RT_B, CT_NONE}},
        {0x69, {IN_LD, AM_R_R, RT_L, RT_C, CT_NONE}},
        {0x6A, {IN_LD, AM_R_R, RT_L, RT_D, CT_NONE}},
        {0x6B, {IN_LD, AM_R_R, RT_L, RT_E, CT_NONE}},
        {0x6C, {IN_LD, AM_R_R, RT_L, RT_H, CT_NONE}},
        {0x6D, {IN_LD, AM_R_R, RT_L, RT_L, CT_NONE}},
        {0x6E, {IN_LD, AM_R_MR, RT_L, RT_HL, CT_NONE}},
        {0x6F, {IN_LD, AM_R_R, RT_L, RT_A, CT_NONE}},

        // 0x70 - 0x7F
        {0x70, {IN_LD, AM_MR_R, RT_HL, RT_B, CT_NONE}},
        {0x71, {IN_LD, AM_MR_R, RT_HL, RT_C, CT_NONE}},
        {0x72, {IN_LD, AM_MR_R, RT_HL, RT_D, CT_NONE}},
        {0x73, {IN_LD, AM_MR_R, RT_HL, RT_E, CT_NONE}},
        {0x74, {IN_LD, AM_MR_R, RT_HL, RT_H, CT_NONE}},
        {0x75, {IN_LD, AM_MR_R, RT_HL, RT_L, CT_NONE}},
        {0x76, {IN_HALT, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0x77, {IN_LD, AM_MR_R, RT_HL, RT_A, CT_NONE}},
        {0x78, {IN_LD, AM_R_R, RT_A, RT_B, CT_NONE}},
        {0x79, {IN_LD, AM_R_R, RT_A, RT_C, CT_NONE}},
        {0x7A, {IN_LD, AM_R_R, RT_A, RT_D, CT_NONE}},
        {0x7B, {IN_LD, AM_R_R, RT_A, RT_E, CT_NONE}},
        {0x7C, {IN_LD, AM_R_R, RT_A, RT_H, CT_NONE}},
        {0x7D, {IN_LD, AM_R_R, RT_A, RT_L, CT_NONE}},
        {0x7E, {IN_LD, AM_R_MR, RT_A, RT_HL, CT_NONE}},
        {0x7F, {IN_LD, AM_R_R, RT_A, RT_A, CT_NONE}},

        // 0x80 - 0x8F
        {0x80, {IN_ADD, AM_R_R, RT_A, RT_B, CT_NONE}},
        {0x81, {IN_ADD, AM_R_R, RT_A, RT_C, CT_NONE}},
        {0x82, {IN_ADD, AM_R_R, RT_A, RT_D, CT_NONE}},
        {0x83, {IN_ADD, AM_R_R, RT_A, RT_E, CT_NONE}},
        {0x84, {IN_ADD, AM_R_R, RT_A, RT_H, CT_NONE}},
        {0x85, {IN_ADD, AM_R_R, RT_A, RT_L, CT_NONE}},
        {0x86, {IN_ADD, AM_R_MR, RT_A, RT_HL, CT_NONE}},
        {0x87, {IN_ADD, AM_R_R, RT_A, RT_A, CT_NONE}},
        {0x88, {IN_ADC, AM_R_R, RT_A, RT_B, CT_NONE}},
        {0x89, {IN_ADC, AM_R_R, RT_A, RT_C, CT_NONE}},
        {0x8A, {IN_ADC, AM_R_R, RT_A, RT_D, CT_NONE}},
        {0x8B, {IN_ADC, AM_R_R, RT_A, RT_E, CT_NONE}},
        {0x8C, {IN_ADC, AM_R_R, RT_A, RT_H, CT_NONE}},
        {0x8D, {IN_ADC, AM_R_R, RT_A, RT_L, CT_NONE}},
        {0x8E, {IN_ADC, AM_R_MR, RT_A, RT_HL, CT_NONE}},
        {0x8F, {IN_ADC, AM_R_R, RT_A, RT_A, CT_NONE}},

        // 0x90 - 0x9F
        {0x90, {IN_SUB, AM_R, RT_B, RT_NONE, CT_NONE}},
        {0x91, {IN_SUB, AM_R, RT_C, RT_NONE, CT_NONE}},
        {0x92, {IN_SUB, AM_R, RT_D, RT_NONE, CT_NONE}},
        {0x93, {IN_SUB, AM_R, RT_E, RT_NONE, CT_NONE}},
        {0x94, {IN_SUB, AM_R, RT_H, RT_NONE, CT_NONE}},
        {0x95, {IN_SUB, AM_R, RT_L, RT_NONE, CT_NONE}},
        {0x96, {IN_SUB, AM_MR, RT_HL, RT_NONE, CT_NONE}},
        {0x97, {IN_SUB, AM_R, RT_A, RT_NONE, CT_NONE}},
        {0x98, {IN_SBC, AM_R_R, RT_A, RT_B, CT_NONE}},
        {0x99, {IN_SBC, AM_R_R, RT_A, RT_C, CT_NONE}},
        {0x9A, {IN_SBC, AM_R_R, RT_A, RT_D, CT_NONE}},
        {0x9B, {IN_SBC, AM_R_R, RT_A, RT_E, CT_NONE}},
        {0x9C, {IN_SBC, AM_R_R, RT_A, RT_H, CT_NONE}},
        {0x9D, {IN_SBC, AM_R_R, RT_A, RT_L, CT_NONE}},
        {0x9E, {IN_SBC, AM_R_MR, RT_A, RT_HL, CT_NONE}},
        {0x9F, {IN_SBC, AM_R_R, RT_A, RT_A, CT_NONE}},

        // 0xA0 - 0xAF
        {0xA0, {IN_AND, AM_R, RT_B, RT_NONE, CT_NONE}},
        {0xA1, {IN_AND, AM_R, RT_C, RT_NONE, CT_NONE}},
        {0xA2, {IN_AND, AM_R, RT_D, RT_NONE, CT_NONE}},
        {0xA3, {IN_AND, AM_R, RT_E, RT_NONE, CT_NONE}},
        {0xA4, {IN_AND, AM_R, RT_H, RT_NONE, CT_NONE}},
        {0xA5, {IN_AND, AM_R, RT_L, RT_NONE, CT_NONE}},
        {0xA6, {IN_AND, AM_MR, RT_HL, RT_NONE, CT_NONE}},
        {0xA7, {IN_AND, AM_R, RT_A, RT_NONE, CT_NONE}},
        {0xA8, {IN_XOR, AM_R, RT_B, RT_NONE, CT_NONE}},
        {0xA9, {IN_XOR, AM_R, RT_C, RT_NONE, CT_NONE}},
        {0xAA, {IN_XOR, AM_R, RT_D, RT_NONE, CT_NONE}},
        {0xAB, {IN_XOR, AM_R, RT_E, RT_NONE, CT_NONE}},
        {0xAC, {IN_XOR, AM_R, RT_H, RT_NONE, CT_NONE}},
        {0xAD, {IN_XOR, AM_R, RT_L, RT_NONE, CT_NONE}},
        {0xAE, {IN_XOR, AM_MR, RT_HL, RT_NONE, CT_NONE}},
        {0xAF, {IN_XOR, AM_R, RT_A, RT_NONE, CT_NONE}},

        // 0xB0 - 0xBF
        {0xB0, {IN_OR, AM_R, RT_B, RT_NONE, CT_NONE}},
        {0xB1, {IN_OR, AM_R, RT_C, RT_NONE, CT_NONE}},
        {0xB2, {IN_OR, AM_R, RT_D, RT_NONE, CT_NONE}},
        {0xB3, {IN_OR, AM_R, RT_E, RT_NONE, CT_NONE}},
        {0xB4, {IN_OR, AM_R, RT_H, RT_NONE, CT_NONE}},
        {0xB5, {IN_OR, AM_R, RT_L, RT_NONE, CT_NONE}},
        {0xB6, {IN_OR, AM_MR, RT_HL, RT_NONE, CT_NONE}},
        {0xB7, {IN_OR, AM_R, RT_A, RT_NONE, CT_NONE}},
        {0xB8, {IN_CP, AM_R, RT_B, RT_NONE, CT_NONE}},
        {0xB9, {IN_CP, AM_R, RT_C, RT_NONE, CT_NONE}},
        {0xBA, {IN_CP, AM_R, RT_D, RT_NONE, CT_NONE}},
        {0xBB, {IN_CP, AM_R, RT_E, RT_NONE, CT_NONE}},
        {0xBC, {IN_CP, AM_R, RT_H, RT_NONE, CT_NONE}},
        {0xBD, {IN_CP, AM_R, RT_L, RT_NONE, CT_NONE}},
        {0xBE, {IN_CP, AM_MR, RT_HL, RT_NONE, CT_NONE}},
        {0xBF, {IN_CP, AM_R, RT_A, RT_NONE, CT_NONE}},

        // 0xC0 - 0xCF
        {0xC0, {IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_NZ}},
        {0xC1, {IN_POP, AM_R, RT_BC, RT_NONE, CT_NONE}},
        {0xC2, {IN_JP, AM_D16, RT_NONE, RT_NONE, CT_NZ}},
        {0xC3, {IN_JP, AM_D16, RT_NONE, RT_NONE, CT_NONE}},
        {0xC4, {IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_NZ}},
        {0xC5, {IN_PUSH, AM_R, RT_BC, RT_NONE, CT_NONE}},
        {0xC6, {IN_ADD, AM_R_D8, RT_A, RT_NONE, CT_NONE}},
        {0xC7, {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xC8, {IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_Z}},
        {0xC9, {IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xCA, {IN_JP, AM_D16, RT_NONE, RT_NONE, CT_Z}},
        {0xCB, {IN_CB, AM_D8, RT_NONE, RT_NONE, CT_NONE}},
        {0xCC, {IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_Z}},
        {0xCD, {IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_NONE}},
        {0xCE, {IN_ADC, AM_R_D8, RT_A, RT_NONE, CT_NONE}},
        {0xCF, {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},

        // 0xD0 - 0xDF
        {0xD0, {IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_NC}},
        {0xD1, {IN_POP, AM_R, RT_DE, RT_NONE, CT_NONE}},
        {0xD2, {IN_JP, AM_D16, RT_NONE, RT_NONE, CT_NC}},
        {0xD3, {IN_ERR, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xD4, {IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_NC}},
        {0xD5, {IN_PUSH, AM_R, RT_DE, RT_NONE, CT_NONE}},
        {0xD6, {IN_SUB, AM_D8, RT_NONE, RT_NONE, CT_NONE}},
        {0xD7, {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xD8, {IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_C}},
        {0xD9, {IN_RETI, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xDA, {IN_JP, AM_D16, RT_NONE, RT_NONE, CT_C}},
        {0xDB, {IN_ERR, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xDC, {IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_C}},
        {0xDD, {IN_ERR, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xDE, {IN_SBC, AM_R_D8, RT_A, RT_NONE, CT_NONE}},
        {0xDF, {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},

        // 0xE0 - 0xEF
        {0xE0, {IN_LDH, AM_A8_R, RT_NONE, RT_A, CT_NONE}},
        {0xE1, {IN_POP, AM_R, RT_HL, RT_NONE, CT_NONE}},
        {0xE2, {IN_LD, AM_MR_R, RT_C, RT_A, CT_NONE}},
        {0xE3, {IN_ERR, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xE4, {IN_ERR, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xE5, {IN_PUSH, AM_R, RT_HL, RT_NONE, CT_NONE}},
        {0xE6, {IN_AND, AM_D8, RT_NONE, RT_NONE, CT_NONE}},
        {0xE7, {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xE8, {IN_ADD, AM_R_D8, RT_SP, RT_NONE, CT_NONE}},
        {0xE9, {IN_JP, AM_R, RT_HL, RT_NONE, CT_NONE}},
        {0xEA, {IN_LD, AM_A16_R, RT_NONE, RT_A, CT_NONE}},
        {0xEB, {IN_ERR, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xEC, {IN_ERR, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xED, {IN_ERR, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xEE, {IN_XOR, AM_D8, RT_NONE, RT_NONE, CT_NONE}},
        {0xEF, {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},

        // 0xF0 - 0xFF
        {0xF0, {IN_LDH, AM_R_A8, RT_A, RT_NONE, CT_NONE}},
        {0xF1, {IN_POP, AM_R, RT_AF, RT_NONE, CT_NONE}},
        {0xF2, {IN_LD, AM_R_MR, RT_A, RT_C, CT_NONE}},
        {0xF3, {IN_DI, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xF4, {IN_ERR, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xF5, {IN_PUSH, AM_R, RT_AF, RT_NONE, CT_NONE}},
        {0xF6, {IN_OR, AM_D8, RT_NONE, RT_NONE, CT_NONE}},
        {0xF7, {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xF8, {IN_LD, AM_HL_SPR, RT_HL, RT_SP, CT_NONE}},
        {0xF9, {IN_LD, AM_R_R, RT_SP, RT_HL, CT_NONE}},
        {0xFA, {IN_LD, AM_R_A16, RT_A, RT_NONE, CT_NONE}},
        {0xFB, {IN_EI, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xFC, {IN_ERR, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xFD, {IN_ERR, AM_IMP, RT_NONE, RT_NONE, CT_NONE}},
        {0xFE, {IN_CP, AM_D8, RT_NONE, RT_NONE, CT_NONE}},
        {0xFF, {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE}}
    };

}

OpcodeInfo CPU::OpcodeToInstruction(uint8_t opcode) const
{
    auto it = this->OpcodeTable.find(opcode);
    if (it != this->OpcodeTable.end()) {
		return it->second; // donne l'instruction correspondante | it->first donne le cl� (l'opcode)
    }

    return { IN_ERR, AM_IMP, RT_NONE, RT_NONE }; // Return an error instruction if not found
}

uint16_t CPU::readRegister(RegType RT)
{
	uint16_t value = 0;
	switch (RT) {
	case RT_A: value = reg.A; break;
    case RT_F: value = reg.F; break;
	case RT_B: value = reg.B; break;
	case RT_C: value = reg.C; break;
	case RT_D: value = reg.D; break;
	case RT_E: value = reg.E; break;
	case RT_H: value = reg.H; break;
	case RT_L: value = reg.L; break;

    case RT_AF: value = (reg.A << 8) | reg.F; break;
    case RT_BC: value = (reg.B << 8) | reg.C; break;
    case RT_DE: value = (reg.D << 8) | reg.E; break;
    case RT_HL: value = (reg.H << 8) | reg.L; break;
    case RT_SP: value = reg.SP; break;
    case RT_PC: value = reg.PC; break;
	default:
		std::cerr << "Invalid register read: " << static_cast<int>(RT) << "\n";
		exit(1);
	}
	return value;
}

void CPU::writeRegister(RegType RT, uint16_t val)
{
    switch (RT)
    {
    case RT_A: reg.A = val; break;
    case RT_F: reg.F = val; break;
    case RT_B: reg.B = val; break;
    case RT_C: reg.C = val; break;
    case RT_D: reg.D = val; break;
    case RT_E: reg.E = val; break;
    case RT_H: reg.H = val; break;
    case RT_L: reg.L = val; break;

    case RT_AF:
        reg.A = val >> 8;
        reg.F = val & 0xFF;
        break;
    case RT_BC:
        reg.B = val >> 8;
        reg.C = val & 0xFF;
        break;
    case RT_DE:
        reg.D = val >> 8;
        reg.E = val & 0xFF;
        break;
    case RT_HL:
        reg.H = val >> 8;
        reg.L = val & 0xFF;
		// printf("H: %02X L: %02X\n", (val >> 4) & 0xff, val & 0xFF);
        break;
    case RT_SP: reg.SP = val; break;
    case RT_PC: reg.PC = val; break;
    default:
        std::cerr << "Invalid register read: " << static_cast<int>(RT) << "\n";
        exit(1);
    }
    return;
}


void CPU::setFlags(int z, int n, int h, int c)
{
    if (z != -1)
    {
		reg.F = (reg.F & ~(1 << 7)) | (z << 7); // Set Z flag
		// std::cout << "Z flag set to: " << static_cast<int>(reg.F) << "\n";
    }

    if (n != -1)
    {
		reg.F = (reg.F & ~(1 << 6)) | (n << 6); // Set N flag
        // std::cout << "n flag set to: " << static_cast<int>(reg.F) << "\n";
    }

    if (h != -1)
    {
		reg.F = (reg.F & ~(1 << 5)) | (h << 5); // Set H flag
		// std::cout << "H flag set to: " << static_cast<int>(reg.F) << "\n";
    }

    if (c != -1)
    {
		reg.F = (reg.F & ~(1 << 4)) | (c << 4); // Set C flag
        // std::cout << "C flag set to: " << static_cast<int>(reg.F) << "\n";
    }
}

void CPU::executeOpcode(uint8_t opcode, Bus& bus)
{
    std::ostringstream oss;
    oss << std::uppercase << std::setfill('0') << std::hex;
    oss << "A:" << std::setw(2) << static_cast<unsigned int>(reg.A) << " ";
    oss << "F:" << std::setw(2) << static_cast<unsigned int>(reg.F) << " ";
    oss << "B:" << std::setw(2) << static_cast<unsigned int>(reg.B) << " ";
    oss << "C:" << std::setw(2) << static_cast<unsigned int>(reg.C) << " ";
    oss << "D:" << std::setw(2) << static_cast<unsigned int>(reg.D) << " ";
    oss << "E:" << std::setw(2) << static_cast<unsigned int>(reg.E) << " ";
    oss << "H:" << std::setw(2) << static_cast<unsigned int>(reg.H) << " ";
    oss << "L:" << std::setw(2) << static_cast<unsigned int>(reg.L) << " ";
    oss << "SP:" << std::setw(4) << reg.SP << " ";
    oss << "PC:" << std::setw(4) << reg.PC << " ";
    oss << "PCMEM:" << std::setw(2) << static_cast<unsigned int>(bus.read(reg.PC)) << "," << std::setw(2) << static_cast<unsigned int>(bus.read(reg.PC + 1)) <<  "," << std::setw(2) << static_cast<unsigned int>(bus.read(reg.PC + 2)) << "," << std::setw(2) << static_cast<unsigned int>(bus.read(reg.PC + 3));
	oss << "\n";
    std::ofstream file("etat_cpu.txt", std::ios::app);
    if (file.is_open())
    {
        file << oss.str();
        file.close();
    }

    uint16_t pc = reg.PC;
	destIsMem = false;
    currentInstruction = OpcodeToInstruction(opcode);
    reg.PC++;
	fetchData(bus);

    std::cout << "0x" << std::hex << static_cast<int>(pc) << ": " << getInstructionName(currentInstruction.IN)
        << " A: " << std::hex << static_cast<int>(reg.A)
        << " F: " << std::hex << static_cast<int>(reg.F)
        << " B: " << std::hex << static_cast<int>(reg.B)
        << " C: " << std::hex << static_cast<int>(reg.C)
        << " D: " << std::hex << static_cast<int>(reg.D)
        << " E: " << std::hex << static_cast<int>(reg.E)
        << " H: " << std::hex << static_cast<int>(reg.H)
        << " L: " << std::hex << static_cast<int>(reg.L)
        << " SP: " << std::hex << static_cast<int>(reg.SP)
        << "\n";

    dbgUpdate(bus);
    dbgPrint();

    switch (currentInstruction.IN)
    {
	case IN_NOP:
		// No operation
		break;
	case IN_LD:
		procLD(bus);
		break;
	case IN_JP:
		procJP();
		break;
	case IN_ADC:
		procADC(bus);
		break;
	case IN_ADD:
		procADD(bus);
		break;
	case IN_SUB:
		procSUB(bus);
		break;
	case IN_SBC:
		procSBC(bus);
		break;
	case IN_AND:
		procAND(bus);
		break;
	case IN_XOR:
		procXOR(bus);
		break;
	case IN_OR:
		procOR(bus);
		break;
	case IN_CP:
		procCP(bus);
		break;
	case IN_INC:
		procINC(bus);
		break;
	case IN_DEC:
		procDEC(bus);
		break;
	case IN_CALL:
		procCALL(bus);
		break;
	case IN_RET:
		procRET(bus);
		break;
	case IN_RST:
		procRST(bus);
		break;
	case IN_RETI:
		procRETI(bus);
		break;
	case IN_POP:
		procPOP(bus);
		break;
	case IN_PUSH:
		procPUSH(bus);
		break;
	case IN_SCF:
		procSCF();
		break;
	case IN_CCF:
		procCCF();
		break;
	case IN_RRCA:
		procRRCA();
		break;
	case IN_RLCA:
		procRLCA();
		break;
	case IN_RRA:
		procRRA();
		break;
	case IN_DAA:
		procDAA();
		break;
	case IN_CPL:
		procCPL();
		break;
	case IN_RLA:
		procRLA();
		break;
	case IN_LDH:
		procLDH(bus);
        break;
    case IN_JR:
        procJR();
		break;
    case IN_DI:
        procDI();
		break;
    case IN_EI:
		procEI();
        break;
    case IN_CB:
        procCB(bus);
        break;
    default:
        std::cerr << "Invalid OPCODE read: " << static_cast<int>(currentInstruction.IN) << "\n";
        exit(1);
    }
}

void CPU::fetchData(Bus& bus)
{
    uint16_t lo;
    uint16_t hi;
    uint16_t addr;
	switch (currentInstruction.AM)
	{
	case AM_IMP: break;
	case AM_R_D16:
        lo = bus.read(reg.PC++);
        hi = bus.read(reg.PC++);

		fetchDataVal = (hi << 8) | lo;
		break;
	case AM_R_D8:
		fetchDataVal = bus.read(reg.PC++);
		break;
	case AM_MR_R:
        fetchDataVal = readRegister(currentInstruction.RT2);
        destMem = readRegister(currentInstruction.RT1);
        destIsMem = true;

        if (currentInstruction.RT1 == RT_C) 
        {
            destMem |= 0xFF00;
        }
		break;
	case AM_R_MR:
        if (currentInstruction.RT2 == RT_C) 
        {
            fetchDataVal = bus.read(readRegister(RT_C) | 0xFF00);
        } 
        else 
        {
            fetchDataVal = bus.read(readRegister(currentInstruction.RT2));
		}
		break;

	case AM_HLI_R:
        fetchDataVal = readRegister(currentInstruction.RT2);
        destMem = readRegister(RT_HL);
        destIsMem = true;
        writeRegister(RT_HL, readRegister(RT_HL) + 1);
		break;
	case AM_HLD_R:
        fetchDataVal = readRegister(currentInstruction.RT2);
        destMem = readRegister(RT_HL);
        destIsMem = true;
        writeRegister(RT_HL, readRegister(RT_HL) - 1);
		break;
    case AM_R_HLI:
		fetchDataVal = bus.read(readRegister(RT_HL));
        writeRegister(RT_HL, readRegister(RT_HL) + 1);
        break;
    case AM_R_HLD:
        fetchDataVal = bus.read(readRegister(RT_HL));
        writeRegister(RT_HL, readRegister(RT_HL) - 1);
        break;
	case AM_R_A8:
		addr = bus.read(reg.PC++) | 0xFF00;
		fetchDataVal = bus.read(addr);
	    std::cout << "A8: " << std::hex << addr << "\n";
		std::cout << "fetchDataVal: " << std::hex << fetchDataVal << "\n";
		break;
	case AM_A8_R:
        destMem = bus.read(reg.PC++);
        destMem |= 0xFF00;
        destIsMem = true;
		fetchDataVal = readRegister(currentInstruction.RT2);
		break;
	case AM_R:
        fetchDataVal = readRegister(currentInstruction.RT1);
        break;
    case AM_D16:
        fetchDataVal = bus.read16(reg.PC++);
        reg.PC++;
        break;
	case AM_R_R:
		fetchDataVal = readRegister(currentInstruction.RT2);
		break;
	case AM_MR:
		destMem = readRegister(currentInstruction.RT1);
		destIsMem = true;
		break;
    case AM_D8:
        fetchDataVal = bus.read(reg.PC++);
		break;
    case AM_A16_R:
        destMem = bus.read16(reg.PC);
		reg.PC += 2;
		destIsMem = true;
		fetchDataVal = readRegister(currentInstruction.RT2);
        break;
    case AM_R_A16:
		fetchDataVal = bus.read16(reg.PC);
		fetchDataVal = bus.read(fetchDataVal);
		reg.PC += 2;
        break;
    case AM_HL_SPR:
		fetchDataVal = bus.read(reg.PC++); 
        break;
	default:
		std::cerr << "Unsupported addressing mode: " << static_cast<int>(currentInstruction.AM) << "\n";
		exit(1);
	}

    return;
}

std::string CPU::getInstructionName(Instruction instr) const
{
    switch (instr)
    {   
    case IN_NONE:
        return "IN_NONE";
        break;
    case IN_NOP:
        return "IN_NOP";
        break;
    case IN_LD:
		return "IN_LD";
        break;
    case IN_INC:
		return "IN_INC";
        break;
    case IN_DEC:
		return "IN_DEC";
        break;
    case IN_RLCA:
		return "IN_RLCA";
        break;
    case IN_ADD:
		return "IN_ADD";
        break;
    case IN_RRCA:
		return "IN_RRCA";
        break;
    case IN_STOP:
		return "IN_STOP";
        break;
    case IN_RLA:
		return "IN_RLA";
        break;
    case IN_JR:
		return "IN_JR";
        break;
    case IN_RRA:
		return "IN_RRA";
        break;
    case IN_DAA:
		return "IN_DAA";
        break;
    case IN_CPL:
		return "IN_CPL";
        break;
    case IN_SCF:
		return "IN_SCF";
        break;
    case IN_CCF:
		return "IN_CCF";
        break;
    case IN_HALT:
		return "IN_HALT";
        break;
    case IN_ADC:
		return "IN_ADC";
        break;
    case IN_SUB:
		return "IN_SUB";
        break;
    case IN_SBC:
		return "IN_SBC";
        break;
    case IN_AND:
		return "IN_AND";
        break;
    case IN_XOR:
		return "IN_XOR";
        break;
    case IN_OR:
		return "IN_OR";
        break;
    case IN_CP:
		return "IN_CP";
        break;
    case IN_POP:
		return "IN_POP";
        break;
    case IN_JP:
		return "IN_JP";
        break;
    case IN_PUSH:
		return "IN_PUSH";
        break;
    case IN_RET:
		return "IN_RET";
        break;
    case IN_CB:
		return "IN_CB";
        break;
    case IN_CALL:
		return "IN_CALL";
        break;
    case IN_RETI:
		return "IN_RETI";
        break;
    case IN_LDH:
		return "IN_LDH";
        break;
    case IN_DI:
		return "IN_DI";
        break;
    case IN_EI:
		return "IN_EI";
        break;
    case IN_RST:
		return "IN_RST";
        break;
    case IN_ERR:
		return "IN_ERR";
        break;
    case IN_RLC:
		return "IN_RLC";
        break;
    case IN_RRC:
		return "IN_RRC";
        break;
    case IN_RL:
		return "IN_RL";
        break;
    case IN_RR:
		return "IN_RR";
        break;
    case IN_SLA:
		return "IN_SLA";
        break;
    case IN_SRA:
		return "IN_SRA";
        break;
    case IN_SWAP:
		return "IN_SWAP";
        break;
    case IN_SRL:
		return "IN_SRL";
        break;
    case IN_BIT:
		return "IN_BIT";
        break;
    case IN_RES:
		return "IN_RES";
        break;
    case IN_SET:
		return "IN_SET";
        break;
    default:
        break;
    }
}

void CPU::procLD(Bus& bus)
{
    if (destIsMem)
    {
        if (currentInstruction.RT2 >= RT_AF)
        {
            bus.write16(destMem, fetchDataVal);
        }
        else
        {
		    bus.write(destMem, fetchDataVal);
        }
    }
    else if (currentInstruction.AM == AM_HL_SPR)
    {
        int cFlag = (((readRegister(currentInstruction.RT2) & 0xFF) + fetchDataVal) > 0xFF);
        int hFlag = ((readRegister(currentInstruction.RT2) & 0xF) + (fetchDataVal & 0xF)) > 0xF;

		setFlags(0, 0, hFlag, cFlag);

        writeRegister(currentInstruction.RT1, readRegister(currentInstruction.RT2) + (char)fetchDataVal);
    }
    else
    {
		writeRegister(currentInstruction.RT1, fetchDataVal);
    }
}

bool CPU::checkCond(CT cond)
{
    bool z = (reg.F & 0x80) >> 7;
    bool c = (reg.F & 0x10) >> 4;
    if (cond == CT_Z)
        return z;
    else if (cond == CT_NZ)
        return !z;
    else if (cond == CT_C)
        return c;
    else if (cond == CT_NC)
        return !c;
    else if (cond == CT_NONE)
        return true;
	else
	{
		std::cerr << "Invalid condition: " << static_cast<int>(cond) << "\n";
		exit(1);
	}
}

void CPU::procJP()
{
    if (checkCond(currentInstruction.condition))
        reg.PC = fetchDataVal;
}

void CPU::procINC(Bus& bus)
{
	if (currentInstruction.RT1 >= RT_AF)
	{
		uint16_t value = readRegister(currentInstruction.RT1) + 1;
		writeRegister(currentInstruction.RT1, value);
        return;
	}
	else if (destIsMem)
	{
		uint8_t value = bus.read(destMem) + 1;
		bus.write(destMem, value & 0xFF);

		bool zFlag = (value == 0);
        bool hFlag = ((value & 0xFF) > 0xF);

        setFlags(zFlag, 0, hFlag, -1);
		return;
	}
    else 
    {
        uint8_t value = readRegister(currentInstruction.RT1) + 1;
        writeRegister(currentInstruction.RT1, value & 0xFF);

        bool zFlag = value == 0;
        bool hFlag = (value & 0x0F) == 0;

		// std::cout << zFlag << "" << hFlag << "\n";

        setFlags(zFlag, 0, hFlag, -1);
    }
}

void CPU::procDEC(Bus& bus)
{
    if (destIsMem)
    {
        uint8_t value = bus.read(destMem) - 1;
        bus.write(destMem, value & 0xFF);

        bool zFlag = (value == 0);
        bool hFlag = (value & 0x0F) == 0x0F;

        setFlags(zFlag, 1, hFlag, -1);
        return;
    }
    else if (currentInstruction.RT1 >= RT_AF)
    {
        uint16_t value = readRegister(currentInstruction.RT1) - 1;
        writeRegister(currentInstruction.RT1, value);
        return;
    }
    else
    {
        uint8_t value = readRegister(currentInstruction.RT1) - 1;
        writeRegister(currentInstruction.RT1, value & 0xFF);

        bool zFlag = (value == 0);
        bool hFlag = (value & 0x0F) == 0x0F;

        setFlags(zFlag, 1, hFlag, -1);
    }
}

void CPU::procADD(Bus& bus)
{

    uint32_t val = readRegister(currentInstruction.RT1) + fetchDataVal;

    bool is_16bit = currentInstruction.RT1 >= RT_AF;

    if (currentInstruction.RT1 == RT_SP) {
        val = readRegister(currentInstruction.RT1) + (char)fetchDataVal;
    }

    int z = (val & 0xFF) == 0;
    int h = (readRegister(currentInstruction.RT1) & 0xF) + (fetchDataVal & 0xF) >= 0x10;
    int c = (int)(readRegister(currentInstruction.RT1) & 0xFF) + (int)(fetchDataVal & 0xFF) >= 0x100;

    if (is_16bit) {
        z = -1;
        h = (readRegister(currentInstruction.RT1) & 0xFFF) + (fetchDataVal & 0xFFF) >= 0x1000;
        uint32_t n = ((uint32_t)readRegister(currentInstruction.RT1)) + ((uint32_t)fetchDataVal);
        c = n >= 0x10000;
    }

    if (currentInstruction.RT1 == RT_SP) {
        z = 0;
        h = (readRegister(currentInstruction.RT1) & 0xF) + (fetchDataVal & 0xF) >= 0x10;
        c = (int)(readRegister(currentInstruction.RT1) & 0xFF) + (int)(fetchDataVal & 0xFF) >= 0x100;
    }

    writeRegister(currentInstruction.RT1, val & 0xFFFF);
    setFlags(z, 0, h, c);
}

void CPU::procJR()
{
    int8_t signed8 = (char)fetchDataVal;

    if (checkCond(currentInstruction.condition))
    {
        reg.PC += signed8;
    }
}

void CPU::procAND(Bus& bus)
{
    if (currentInstruction.AM == AM_MR)
    {
        fetchDataVal = bus.read(destMem);
    }

	reg.A &= fetchDataVal;
	bool zFlag = (reg.A == 0);
	setFlags(zFlag, 0, 1, 0);
}
void CPU::procOR(Bus& bus)
{
    if (currentInstruction.AM == AM_MR)
    {
        fetchDataVal = bus.read(destMem);
    }

    reg.A |= fetchDataVal;
    bool zFlag = (reg.A == 0);
    setFlags(zFlag, 0, 0, 0);
}

void CPU::procXOR(Bus& bus)
{
    if (currentInstruction.AM == AM_MR)
    {
        fetchDataVal = bus.read(destMem);
    }

    reg.A ^= fetchDataVal;
    bool zFlag = (reg.A == 0);
    setFlags(zFlag, 0, 0, 0);
}

void CPU::procCP(Bus& bus)
{
    if (currentInstruction.AM == AM_MR)
    {
        fetchDataVal = bus.read(destMem);
    }

	uint8_t value = reg.A - fetchDataVal;
	bool zFlag = (value == 0);
	bool hFlag = ((reg.A & 0xF) < (fetchDataVal & 0xF));
	bool cFlag = (reg.A < fetchDataVal);
	setFlags(zFlag, 1, hFlag, cFlag);
}

void CPU::procADC(Bus& bus)
{
    if (currentInstruction.AM == AM_R_MR)
    {
        fetchDataVal = bus.read(destMem);
    }

    uint16_t res = reg.A + fetchDataVal + ((reg.F & 0x10) >> 4); // Add A, fetchDataVal and carry if set
    bool zFlag = (res & 0xFF) == 0;
    bool hFlag = (reg.A & 0xF) + (fetchDataVal & 0xF) + ((reg.F & 0x10) >> 4) > 0xF;
    bool cFlag = res > 0xFF;
	reg.A = res & 0xFF; // Store result in A
    setFlags(zFlag, 0, hFlag, cFlag);
}

void CPU::procSUB(Bus& bus)
{
    if (currentInstruction.AM == AM_MR)
    {
        fetchDataVal = bus.read(destMem);
    }

    uint16_t value = readRegister(RT_A) - fetchDataVal;
    bool hFlag = (readRegister(RT_A) & 0xF) - (fetchDataVal & 0xF) < 0;
    bool cFlag = (readRegister(RT_A) - fetchDataVal) < 0;
    bool zFlag = value == 0;
    setFlags(zFlag, 1, hFlag, cFlag);
    writeRegister(RT_A, value & 0xFFFF);
}
void CPU::procSBC(Bus& bus)
{
    uint16_t value = readRegister(RT_A) - fetchDataVal - ((reg.F & 0x10) >> 4);
    bool hFlag = (readRegister(RT_A) & 0xF) - (fetchDataVal & 0xF) < 0;
    bool cFlag = (readRegister(RT_A) - fetchDataVal) < 0;
    bool zFlag = value == 0;
    setFlags(zFlag, 1, hFlag, cFlag);
    writeRegister(RT_A, value & 0xFFFF);
}

void CPU::procPOP(Bus& bus)
{
    if (currentInstruction.RT1 == RT_AF) {
		uint16_t lo = stack->pop(reg, bus);
		uint16_t hi = stack->pop(reg, bus);
        uint16_t res = (hi << 8) | lo;
        writeRegister(currentInstruction.RT1, res & 0xFFF0);
        return;
    }

	writeRegister(currentInstruction.RT1 ,stack->pop16(reg, bus));
}
void CPU::procPUSH(Bus& bus)
{
    stack->push16(readRegister(currentInstruction.RT1), reg, bus);
}

void CPU::procCALL(Bus& bus)
{
    if (checkCond(currentInstruction.condition))
    {
        stack->push16(reg.PC, reg, bus);
		reg.PC = fetchDataVal;
        return;
    }
}
void CPU::procRET(Bus& bus)
{
    if (checkCond(currentInstruction.condition))
    {
        reg.PC = stack->pop16(reg, bus);
    }
}

void CPU::procRST(Bus& bus)
{
    stack->push16(reg.PC, reg, bus);
	uint8_t n = (bus.read(reg.PC - 1) & 0x38);
    reg.PC = n;
}

void CPU::procSCF()
{
    setFlags(-1, 0, 0, 1);
}

void CPU::procCCF()
{
    setFlags(-1, 0, 0, ((reg.F & 0x10) >> 4) ^ 1);
}

void CPU::procRETI(Bus& bus)
{
	procRET(bus);
	reg.IME = true;
}

void CPU::procRLCA()
{
	uint8_t aTemp = reg.A;
    uint8_t c = (aTemp >> 7) & 1;
    aTemp = (aTemp << 1) | c;
    reg.A = aTemp;

    setFlags(0, 0, 0, c);
}
void CPU::procRRCA()
{
    uint8_t c = reg.A & 1;
    reg.A = reg.A >> 1;
    reg.A |= (c << 7);

    setFlags(0, 0, 0, c);
}

void CPU::procRRA()
{
    uint8_t c = reg.F & 0x10;
    uint8_t newC = reg.A & 1;

    reg.A = reg.A >> 1;
    reg.A |= (c << 3);

    setFlags(0, 0, 0, newC);
}
void CPU::procDAA()
{
    uint8_t u = 0;
    int fc = 0;

	bool flagC = (reg.F & 0x10) >> 4;
    bool flagH = (reg.F & 0x20) >> 5;
    bool flagN = (reg.F & 0x40) >> 6;

    if (flagH || (!flagN && (reg.A & 0xF) > 9)) {
        u = 6;
    }

    if (flagC || (!flagN && reg.A > 0x99)) {
        u |= 0x60;
        fc = 1;
    }

    reg.A += flagN ? -u : u;

    setFlags(reg.A == 0, -1, 0, fc);
}
void CPU::procCPL()
{
    reg.A = ~reg.A;
	setFlags(-1, 1, 1, -1); 
}

void CPU::procRLA()
{
    uint8_t aTemp = reg.A;
    uint8_t cFlag = (reg.F & 0x10) >> 4;
    uint8_t c = (aTemp >> 7) & 1;

    reg.A = (aTemp << 1) | cFlag;
    setFlags(0, 0, 0, c);
}

void CPU::procLDH(Bus& bus)
{
    if (destIsMem)
    {
		bus.write(destMem, fetchDataVal);

    }
    else
    {
        reg.A = fetchDataVal;
    }
}

void CPU::procDI()
{
	reg.IME = false;
}

void CPU::procEI()
{
	reg.IME = true;
}

void CPU::dbgUpdate(Bus& bus)
{
    if (bus.read(0xFF02) == 0x81)
    { // Check if serial data is ready
        char c = bus.read(0xFF01); // Read serial data

        msg[msglgth++] = c;
        bus.write(0xFF02, 0); // Clear the serial control register
    }
}

void CPU::dbgPrint()
{
    if (msg[0])
    {
        std::cout << "DBG: " << msg;
    }
}

RegType CPU::getRegType(uint8_t regCode) const
{
    switch (regCode)
    {
    case 0: return RT_B;
    case 1: return RT_C;
    case 2: return RT_D;
    case 3: return RT_E;
    case 4: return RT_H;
    case 5: return RT_L;
    case 6: return RT_HL; // Memory read
    case 7: return RT_A;
    default:
        std::cerr << "Invalid register code: " << static_cast<int>(regCode) << "\n";
        exit(1);
	}
}

void CPU::procCB(Bus& bus)
{
	RegType operand = getRegType(fetchDataVal & 0x07);
	uint8_t bitIndex = (fetchDataVal >> 3) & 0x07;
	uint8_t brs = (fetchDataVal & 0xC0) >> 6;
	uint8_t ssss = (fetchDataVal & 0x38) >> 3;
    uint8_t rrrr = (fetchDataVal & 0x18) >> 3;

    uint8_t mask;
    uint8_t oldRegVal;

    switch (brs)
    {
        case 1: // BIT
			mask = 1 << bitIndex;
            if (operand == RT_HL) // Memory read
            {
                uint8_t value = bus.read(readRegister(RT_HL));
                bool bitSet = (value & mask) == 0;
                setFlags(bitSet, 0, 1, -1);
            }
            else // Register read
            {
                uint8_t value = readRegister(operand);
                bool bitSet = (value & mask) == 0;
                setFlags(bitSet, 0, 1, -1); // Set Z, N, H flags
            }
            return;
		case 2: // RES
            mask = 1 << bitIndex;
            if (operand == RT_HL)
            {
                uint8_t value = bus.read(readRegister(RT_HL));
				bus.write(readRegister(RT_HL), value ^ mask); // Clear the bit
            }
            else
            {
                writeRegister(operand, (readRegister(operand) ^ mask));
            }
            return;
        case 3: // SET
            mask = 1 << bitIndex;
            if (operand == RT_HL)
            {
                uint8_t value = bus.read(readRegister(RT_HL));
                bus.write(readRegister(RT_HL), value | mask);
            }
            else
            {
                writeRegister(operand, (readRegister(operand) | mask));
            }
            return;
    }
    switch (ssss)
    {
        case 4: // SLA
            if (operand == RT_HL)
            {
                oldRegVal = bus.read(readRegister(operand));
                bus.write(readRegister(operand), (bus.read(readRegister(operand) << 1)));
            }
            else
            {
                oldRegVal = readRegister(operand);
                writeRegister(operand, (readRegister(operand) << 1));
            }

            setFlags(readRegister(operand) == 0, false, false, !!(oldRegVal & 0x80));
            return;
        case 5: // SRA
            if (operand == RT_HL)
            {
                oldRegVal = bus.read(readRegister(operand));
                bus.write(readRegister(operand), (bus.read(readRegister(operand) >> 1)));
            }
            else
            {
                oldRegVal = readRegister(operand);
                writeRegister(operand, (readRegister(operand) >> 1));
            }

            setFlags(readRegister(operand) == 0, 0, 0, (oldRegVal & 0x01));
            return;
        case 6: // SWAP
            if (operand == RT_HL)
            {
                oldRegVal = bus.read(readRegister(operand));
                uint8_t res = (oldRegVal >> 4) | (oldRegVal << 4);
                bus.write(readRegister(operand), res);
                setFlags(res == 0, 0, 0, 0);
            }
            else
            {
                oldRegVal = readRegister(operand);
                uint8_t res = (oldRegVal >> 4) | (oldRegVal << 4);
                writeRegister(operand, res);
                setFlags(res == 0, 0, 0, 0);
            }
            return;
        case 7: // SRL
            if (operand == RT_HL)
            {
                oldRegVal = bus.read(readRegister(operand));
                uint8_t res = oldRegVal >> 1;
                bus.write(readRegister(operand), res);
                setFlags(res == 0, 0, 0, oldRegVal & 0x01);
            }
            else
            {
                oldRegVal = readRegister(operand);
                uint8_t res = oldRegVal >> 1;
                writeRegister(operand, res);
                setFlags(res == 0, 0, 0, oldRegVal & 0x01);
            }
            return;
    }

    switch (rrrr)
    {
        case 0: // RLC
            if (operand == RT_HL)
            {
                oldRegVal = bus.read(readRegister(operand));
                uint8_t res = oldRegVal << 1;
                bus.write(readRegister(operand), res);
                setFlags(res == 0, 0, 0, oldRegVal & 0x80);
            }
            else
            {
                oldRegVal = readRegister(operand);
                uint8_t res = oldRegVal << 1;
                writeRegister(operand, res);
                setFlags(res == 0, 0, 0, oldRegVal & 0x80);
            }
            return;
        case 1: // RRC
            if (operand == RT_HL)
            {
                oldRegVal = bus.read(readRegister(operand));
                uint8_t res = oldRegVal >> 1;
                bus.write(readRegister(operand), res);
                setFlags(res == 0, 0, 0, oldRegVal & 0x01);
            }
            else
            {
                oldRegVal = readRegister(operand);
                uint8_t res = oldRegVal >> 1;
                writeRegister(operand, res);
                setFlags(res == 0, 0, 0, oldRegVal & 0x01);
            }
            return;
        case 2: // RL
            if (operand == RT_HL)
            {
                oldRegVal = bus.read(readRegister(operand));
                uint8_t res = oldRegVal << 1;
                bus.write(readRegister(operand), res);
                setFlags(res == 0, 0, 0, oldRegVal & 0x80);
            }
            else
            {
                oldRegVal = readRegister(operand);
                uint8_t res = oldRegVal << 1;
                writeRegister(operand, res);
                setFlags(res == 0, 0, 0, oldRegVal & 0x80);
            }
            return;
        case 3: // RR
            if (operand == RT_HL)
            {
                oldRegVal = bus.read(readRegister(operand));
                uint8_t res = oldRegVal >> 1;
                bus.write(readRegister(operand), res);
                setFlags(res == 0, 0, 0, oldRegVal & 0x01);
            }
            else
            {
                oldRegVal = readRegister(operand);
                uint8_t res = (oldRegVal >> 1) | ((reg.F & 0x10) << 3);
                writeRegister(operand, res);
                setFlags(res == 0, 0, 0, oldRegVal & 0x01);
            }
            return;
    }

}