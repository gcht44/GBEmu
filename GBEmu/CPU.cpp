#include "CPU.hpp"



CPU::CPU()
{
    reg = { 0 };
    reg.PC = 0x100;
	halted = false;
	running = true;
	destIsMem = false;

    OpcodeTable = {
            // 0x00 - 0x0F
            {0x00, {IN_NOP, AM_IMP, RT_NONE, RT_NONE}},
            {0x01, {IN_LD, AM_R_D16, RT_BC, RT_NONE}},
            {0x02, {IN_LD, AM_MR_R, RT_BC, RT_A}},
            {0x03, {IN_INC, AM_R, RT_BC, RT_NONE}},
            {0x04, {IN_INC, AM_R, RT_B, RT_NONE}},
            {0x05, {IN_DEC, AM_R, RT_B, RT_NONE}},
            {0x06, {IN_LD, AM_R_D8, RT_B, RT_NONE}},
            {0x07, {IN_RLCA, AM_IMP, RT_NONE, RT_NONE}},
            {0x08, {IN_LD, AM_A16_R, RT_NONE, RT_SP}},
            {0x09, {IN_ADD, AM_R_R, RT_HL, RT_BC}},
            {0x0A, {IN_LD, AM_R_MR, RT_A, RT_BC}},
            {0x0B, {IN_DEC, AM_R, RT_BC, RT_NONE}},
            {0x0C, {IN_INC, AM_R, RT_C, RT_NONE}},
            {0x0D, {IN_DEC, AM_R, RT_C, RT_NONE}},
            {0x0E, {IN_LD, AM_R_D8, RT_C, RT_NONE}},
            {0x0F, {IN_RRCA, AM_IMP, RT_NONE, RT_NONE}},

            // 0x10 - 0x1F
            {0x10, {IN_STOP, AM_D8, RT_NONE, RT_NONE}},
            {0x11, {IN_LD, AM_R_D16, RT_DE, RT_NONE}},
            {0x12, {IN_LD, AM_MR_R, RT_DE, RT_A}},
            {0x13, {IN_INC, AM_R, RT_DE, RT_NONE}},
            {0x14, {IN_INC, AM_R, RT_D, RT_NONE}},
            {0x15, {IN_DEC, AM_R, RT_D, RT_NONE}},
            {0x16, {IN_LD, AM_R_D8, RT_D, RT_NONE}},
            {0x17, {IN_RLA, AM_IMP, RT_NONE, RT_NONE}},
            {0x18, {IN_JR, AM_D8, RT_NONE, RT_NONE}},
            {0x19, {IN_ADD, AM_R_R, RT_HL, RT_DE}},
            {0x1A, {IN_LD, AM_R_MR, RT_A, RT_DE}},
            {0x1B, {IN_DEC, AM_R, RT_DE, RT_NONE}},
            {0x1C, {IN_INC, AM_R, RT_E, RT_NONE}},
            {0x1D, {IN_DEC, AM_R, RT_E, RT_NONE}},
            {0x1E, {IN_LD, AM_R_D8, RT_E, RT_NONE}},
            {0x1F, {IN_RRA, AM_IMP, RT_NONE, RT_NONE}},

            // 0x20 - 0x2F
            {0x20, {IN_JR, AM_D8, RT_NONE, RT_NONE}}, // JR NZ,r8
            {0x21, {IN_LD, AM_R_D16, RT_HL, RT_NONE}},
            {0x22, {IN_LD, AM_HLI_R, RT_NONE, RT_A}},
            {0x23, {IN_INC, AM_R, RT_HL, RT_NONE}},
            {0x24, {IN_INC, AM_R, RT_H, RT_NONE}},
            {0x25, {IN_DEC, AM_R, RT_H, RT_NONE}},
            {0x26, {IN_LD, AM_R_D8, RT_H, RT_NONE}},
            {0x27, {IN_DAA, AM_IMP, RT_NONE, RT_NONE}},
            {0x28, {IN_JR, AM_D8, RT_NONE, RT_NONE}}, // JR Z,r8
            {0x29, {IN_ADD, AM_R_R, RT_HL, RT_HL}},
            {0x2A, {IN_LD, AM_R_HLI, RT_A, RT_NONE}},
            {0x2B, {IN_DEC, AM_R, RT_HL, RT_NONE}},
            {0x2C, {IN_INC, AM_R, RT_L, RT_NONE}},
            {0x2D, {IN_DEC, AM_R, RT_L, RT_NONE}},
            {0x2E, {IN_LD, AM_R_D8, RT_L, RT_NONE}},
            {0x2F, {IN_CPL, AM_IMP, RT_NONE, RT_NONE}},

            // 0x30 - 0x3F
            {0x30, {IN_JR, AM_D8, RT_NONE, RT_NONE}}, // JR NC,r8
            {0x31, {IN_LD, AM_R_D16, RT_SP, RT_NONE}},
            {0x32, {IN_LD, AM_HLD_R, RT_NONE, RT_A}},
            {0x33, {IN_INC, AM_R, RT_SP, RT_NONE}},
            {0x34, {IN_INC, AM_MR, RT_HL, RT_NONE}},
            {0x35, {IN_DEC, AM_MR, RT_HL, RT_NONE}},
            {0x36, {IN_LD, AM_MR_D8, RT_HL, RT_NONE}},
            {0x37, {IN_SCF, AM_IMP, RT_NONE, RT_NONE}},
            {0x38, {IN_JR, AM_D8, RT_NONE, RT_NONE}}, // JR C,r8
            {0x39, {IN_ADD, AM_R_R, RT_HL, RT_SP}},
            {0x3A, {IN_LD, AM_R_HLD, RT_A, RT_NONE}},
            {0x3B, {IN_DEC, AM_R, RT_SP, RT_NONE}},
            {0x3C, {IN_INC, AM_R, RT_A, RT_NONE}},
            {0x3D, {IN_DEC, AM_R, RT_A, RT_NONE}},
            {0x3E, {IN_LD, AM_R_D8, RT_A, RT_NONE}},
            {0x3F, {IN_CCF, AM_IMP, RT_NONE, RT_NONE}},

            // 0x40 - 0x4F (LD r,r)
            {0x40, {IN_LD, AM_R_R, RT_B, RT_B}},
            {0x41, {IN_LD, AM_R_R, RT_B, RT_C}},
            {0x42, {IN_LD, AM_R_R, RT_B, RT_D}},
            {0x43, {IN_LD, AM_R_R, RT_B, RT_E}},
            {0x44, {IN_LD, AM_R_R, RT_B, RT_H}},
            {0x45, {IN_LD, AM_R_R, RT_B, RT_L}},
            {0x46, {IN_LD, AM_R_MR, RT_B, RT_HL}},
            {0x47, {IN_LD, AM_R_R, RT_B, RT_A}},
            {0x48, {IN_LD, AM_R_R, RT_C, RT_B}},
            {0x49, {IN_LD, AM_R_R, RT_C, RT_C}},
            {0x4A, {IN_LD, AM_R_R, RT_C, RT_D}},
            {0x4B, {IN_LD, AM_R_R, RT_C, RT_E}},
            {0x4C, {IN_LD, AM_R_R, RT_C, RT_H}},
            {0x4D, {IN_LD, AM_R_R, RT_C, RT_L}},
            {0x4E, {IN_LD, AM_R_MR, RT_C, RT_HL}},
            {0x4F, {IN_LD, AM_R_R, RT_C, RT_A}},

            // 0x50 - 0x5F
            {0x50, {IN_LD, AM_R_R, RT_D, RT_B}},
            {0x51, {IN_LD, AM_R_R, RT_D, RT_C}},
            {0x52, {IN_LD, AM_R_R, RT_D, RT_D}},
            {0x53, {IN_LD, AM_R_R, RT_D, RT_E}},
            {0x54, {IN_LD, AM_R_R, RT_D, RT_H}},
            {0x55, {IN_LD, AM_R_R, RT_D, RT_L}},
            {0x56, {IN_LD, AM_R_MR, RT_D, RT_HL}},
            {0x57, {IN_LD, AM_R_R, RT_D, RT_A}},
            {0x58, {IN_LD, AM_R_R, RT_E, RT_B}},
            {0x59, {IN_LD, AM_R_R, RT_E, RT_C}},
            {0x5A, {IN_LD, AM_R_R, RT_E, RT_D}},
            {0x5B, {IN_LD, AM_R_R, RT_E, RT_E}},
            {0x5C, {IN_LD, AM_R_R, RT_E, RT_H}},
            {0x5D, {IN_LD, AM_R_R, RT_E, RT_L}},
            {0x5E, {IN_LD, AM_R_MR, RT_E, RT_HL}},
            {0x5F, {IN_LD, AM_R_R, RT_E, RT_A}},

            // 0x60 - 0x6F
            {0x60, {IN_LD, AM_R_R, RT_H, RT_B}},
            {0x61, {IN_LD, AM_R_R, RT_H, RT_C}},
            {0x62, {IN_LD, AM_R_R, RT_H, RT_D}},
            {0x63, {IN_LD, AM_R_R, RT_H, RT_E}},
            {0x64, {IN_LD, AM_R_R, RT_H, RT_H}},
            {0x65, {IN_LD, AM_R_R, RT_H, RT_L}},
            {0x66, {IN_LD, AM_R_MR, RT_H, RT_HL}},
            {0x67, {IN_LD, AM_R_R, RT_H, RT_A}},
            {0x68, {IN_LD, AM_R_R, RT_L, RT_B}},
            {0x69, {IN_LD, AM_R_R, RT_L, RT_C}},
            {0x6A, {IN_LD, AM_R_R, RT_L, RT_D}},
            {0x6B, {IN_LD, AM_R_R, RT_L, RT_E}},
            {0x6C, {IN_LD, AM_R_R, RT_L, RT_H}},
            {0x6D, {IN_LD, AM_R_R, RT_L, RT_L}},
            {0x6E, {IN_LD, AM_R_MR, RT_L, RT_HL}},
            {0x6F, {IN_LD, AM_R_R, RT_L, RT_A}},

            // 0x70 - 0x7F
            {0x70, {IN_LD, AM_MR_R, RT_HL, RT_B}},
            {0x71, {IN_LD, AM_MR_R, RT_HL, RT_C}},
            {0x72, {IN_LD, AM_MR_R, RT_HL, RT_D}},
            {0x73, {IN_LD, AM_MR_R, RT_HL, RT_E}},
            {0x74, {IN_LD, AM_MR_R, RT_HL, RT_H}},
            {0x75, {IN_LD, AM_MR_R, RT_HL, RT_L}},
            {0x76, {IN_HALT, AM_IMP, RT_NONE, RT_NONE}},
            {0x77, {IN_LD, AM_MR_R, RT_HL, RT_A}},
            {0x78, {IN_LD, AM_R_R, RT_A, RT_B}},
            {0x79, {IN_LD, AM_R_R, RT_A, RT_C}},
            {0x7A, {IN_LD, AM_R_R, RT_A, RT_D}},
            {0x7B, {IN_LD, AM_R_R, RT_A, RT_E}},
            {0x7C, {IN_LD, AM_R_R, RT_A, RT_H}},
            {0x7D, {IN_LD, AM_R_R, RT_A, RT_L}},
            {0x7E, {IN_LD, AM_R_MR, RT_A, RT_HL}},
            {0x7F, {IN_LD, AM_R_R, RT_A, RT_A}},

            // 0x80 - 0x8F (ADD/ADC)
            {0x80, {IN_ADD, AM_R_R, RT_A, RT_B}},
            {0x81, {IN_ADD, AM_R_R, RT_A, RT_C}},
            {0x82, {IN_ADD, AM_R_R, RT_A, RT_D}},
            {0x83, {IN_ADD, AM_R_R, RT_A, RT_E}},
            {0x84, {IN_ADD, AM_R_R, RT_A, RT_H}},
            {0x85, {IN_ADD, AM_R_R, RT_A, RT_L}},
            {0x86, {IN_ADD, AM_R_MR, RT_A, RT_HL}},
            {0x87, {IN_ADD, AM_R_R, RT_A, RT_A}},
            {0x88, {IN_ADC, AM_R_R, RT_A, RT_B}},
            {0x89, {IN_ADC, AM_R_R, RT_A, RT_C}},
            {0x8A, {IN_ADC, AM_R_R, RT_A, RT_D}},
            {0x8B, {IN_ADC, AM_R_R, RT_A, RT_E}},
            {0x8C, {IN_ADC, AM_R_R, RT_A, RT_H}},
            {0x8D, {IN_ADC, AM_R_R, RT_A, RT_L}},
            {0x8E, {IN_ADC, AM_R_MR, RT_A, RT_HL}},
            {0x8F, {IN_ADC, AM_R_R, RT_A, RT_A}},

            // 0x90 - 0x9F (SUB/SBC)
            {0x90, {IN_SUB, AM_R, RT_B, RT_NONE}},
            {0x91, {IN_SUB, AM_R, RT_C, RT_NONE}},
            {0x92, {IN_SUB, AM_R, RT_D, RT_NONE}},
            {0x93, {IN_SUB, AM_R, RT_E, RT_NONE}},
            {0x94, {IN_SUB, AM_R, RT_H, RT_NONE}},
            {0x95, {IN_SUB, AM_R, RT_L, RT_NONE}},
            {0x96, {IN_SUB, AM_MR, RT_HL, RT_NONE}},
            {0x97, {IN_SUB, AM_R, RT_A, RT_NONE}},
            {0x98, {IN_SBC, AM_R_R, RT_A, RT_B}},
            {0x99, {IN_SBC, AM_R_R, RT_A, RT_C}},
            {0x9A, {IN_SBC, AM_R_R, RT_A, RT_D}},
            {0x9B, {IN_SBC, AM_R_R, RT_A, RT_E}},
            {0x9C, {IN_SBC, AM_R_R, RT_A, RT_H}},
            {0x9D, {IN_SBC, AM_R_R, RT_A, RT_L}},
            {0x9E, {IN_SBC, AM_R_MR, RT_A, RT_HL}},
            {0x9F, {IN_SBC, AM_R_R, RT_A, RT_A}},

            // 0xA0 - 0xAF (AND/XOR)
            {0xA0, {IN_AND, AM_R, RT_B, RT_NONE}},
            {0xA1, {IN_AND, AM_R, RT_C, RT_NONE}},
            {0xA2, {IN_AND, AM_R, RT_D, RT_NONE}},
            {0xA3, {IN_AND, AM_R, RT_E, RT_NONE}},
            {0xA4, {IN_AND, AM_R, RT_H, RT_NONE}},
            {0xA5, {IN_AND, AM_R, RT_L, RT_NONE}},
            {0xA6, {IN_AND, AM_MR, RT_HL, RT_NONE}},
            {0xA7, {IN_AND, AM_R, RT_A, RT_NONE}},
            {0xA8, {IN_XOR, AM_R, RT_B, RT_NONE}},
            {0xA9, {IN_XOR, AM_R, RT_C, RT_NONE}},
            {0xAA, {IN_XOR, AM_R, RT_D, RT_NONE}},
            {0xAB, {IN_XOR, AM_R, RT_E, RT_NONE}},
            {0xAC, {IN_XOR, AM_R, RT_H, RT_NONE}},
            {0xAD, {IN_XOR, AM_R, RT_L, RT_NONE}},
            {0xAE, {IN_XOR, AM_MR, RT_HL, RT_NONE}},
            {0xAF, {IN_XOR, AM_R, RT_A, RT_NONE}},

            // 0xB0 - 0xBF (OR/CP)
            {0xB0, {IN_OR, AM_R, RT_B, RT_NONE}},
            {0xB1, {IN_OR, AM_R, RT_C, RT_NONE}},
            {0xB2, {IN_OR, AM_R, RT_D, RT_NONE}},
            {0xB3, {IN_OR, AM_R, RT_E, RT_NONE}},
            {0xB4, {IN_OR, AM_R, RT_H, RT_NONE}},
            {0xB5, {IN_OR, AM_R, RT_L, RT_NONE}},
            {0xB6, {IN_OR, AM_MR, RT_HL, RT_NONE}},
            {0xB7, {IN_OR, AM_R, RT_A, RT_NONE}},
            {0xB8, {IN_CP, AM_R, RT_B, RT_NONE}},
            {0xB9, {IN_CP, AM_R, RT_C, RT_NONE}},
            {0xBA, {IN_CP, AM_R, RT_D, RT_NONE}},
            {0xBB, {IN_CP, AM_R, RT_E, RT_NONE}},
            {0xBC, {IN_CP, AM_R, RT_H, RT_NONE}},
            {0xBD, {IN_CP, AM_R, RT_L, RT_NONE}},
            {0xBE, {IN_CP, AM_MR, RT_HL, RT_NONE}},
            {0xBF, {IN_CP, AM_R, RT_A, RT_NONE}},

            // 0xC0 - 0xCF
            {0xC0, {IN_RET, AM_IMP, RT_NONE, RT_NONE}}, // RET NZ
            {0xC1, {IN_POP, AM_R, RT_BC, RT_NONE}},
            {0xC2, {IN_JP, AM_D16, RT_NONE, RT_NONE}}, // JP NZ,a16
            {0xC3, {IN_JP, AM_D16, RT_NONE, RT_NONE}},
            {0xC4, {IN_CALL, AM_D16, RT_NONE, RT_NONE}}, // CALL NZ,a16
            {0xC5, {IN_PUSH, AM_R, RT_BC, RT_NONE}},
            {0xC6, {IN_ADD, AM_R_D8, RT_A, RT_NONE}},
            {0xC7, {IN_RST, AM_IMP, RT_NONE, RT_NONE}}, // RST 00H
            {0xC8, {IN_RET, AM_IMP, RT_NONE, RT_NONE}}, // RET Z
            {0xC9, {IN_RET, AM_IMP, RT_NONE, RT_NONE}},
            {0xCA, {IN_JP, AM_D16, RT_NONE, RT_NONE}}, // JP Z,a16
            {0xCB, {IN_CB, AM_D8, RT_NONE, RT_NONE}},
            {0xCC, {IN_CALL, AM_D16, RT_NONE, RT_NONE}}, // CALL Z,a16
            {0xCD, {IN_CALL, AM_D16, RT_NONE, RT_NONE}},
            {0xCE, {IN_ADC, AM_R_D8, RT_A, RT_NONE}},
            {0xCF, {IN_RST, AM_IMP, RT_NONE, RT_NONE}}, // RST 08H

            // 0xD0 - 0xDF
            {0xD0, {IN_RET, AM_IMP, RT_NONE, RT_NONE}}, // RET NC
            {0xD1, {IN_POP, AM_R, RT_DE, RT_NONE}},
            {0xD2, {IN_JP, AM_D16, RT_NONE, RT_NONE}}, // JP NC,a16
            {0xD3, {IN_ERR, AM_IMP, RT_NONE, RT_NONE}}, // Invalid
            {0xD4, {IN_CALL, AM_D16, RT_NONE, RT_NONE}}, // CALL NC,a16
            {0xD5, {IN_PUSH, AM_R, RT_DE, RT_NONE}},
            {0xD6, {IN_SUB, AM_D8, RT_NONE, RT_NONE}},
            {0xD7, {IN_RST, AM_IMP, RT_NONE, RT_NONE}}, // RST 10H
            {0xD8, {IN_RET, AM_IMP, RT_NONE, RT_NONE}}, // RET C
            {0xD9, {IN_RETI, AM_IMP, RT_NONE, RT_NONE}},
            {0xDA, {IN_JP, AM_D16, RT_NONE, RT_NONE}}, // JP C,a16
            {0xDB, {IN_ERR, AM_IMP, RT_NONE, RT_NONE}}, // Invalid
            {0xDC, {IN_CALL, AM_D16, RT_NONE, RT_NONE}}, // CALL C,a16
            {0xDD, {IN_ERR, AM_IMP, RT_NONE, RT_NONE}}, // Invalid
            {0xDE, {IN_SBC, AM_R_D8, RT_A, RT_NONE}},
            {0xDF, {IN_RST, AM_IMP, RT_NONE, RT_NONE}}, // RST 18H

            // 0xE0 - 0xEF
            {0xE0, {IN_LDH, AM_A8_R, RT_NONE, RT_A}},
            {0xE1, {IN_POP, AM_R, RT_HL, RT_NONE}},
            {0xE2, {IN_LD, AM_MR_R, RT_C, RT_A}}, // LD (C),A
            {0xE3, {IN_ERR, AM_IMP, RT_NONE, RT_NONE}}, // Invalid
            {0xE4, {IN_ERR, AM_IMP, RT_NONE, RT_NONE}}, // Invalid
            {0xE5, {IN_PUSH, AM_R, RT_HL, RT_NONE}},
            {0xE6, {IN_AND, AM_D8, RT_NONE, RT_NONE}},
            {0xE7, {IN_RST, AM_IMP, RT_NONE, RT_NONE}}, // RST 20H
            {0xE8, {IN_ADD, AM_HL_SPR, RT_SP, RT_NONE}},
            {0xE9, {IN_JPHL, AM_IMP, RT_NONE, RT_NONE}},
            {0xEA, {IN_LD, AM_A16_R, RT_NONE, RT_A}},
            {0xEB, {IN_ERR, AM_IMP, RT_NONE, RT_NONE}}, // Invalid
            {0xEC, {IN_ERR, AM_IMP, RT_NONE, RT_NONE}}, // Invalid
            {0xED, {IN_ERR, AM_IMP, RT_NONE, RT_NONE}}, // Invalid
            {0xEE, {IN_XOR, AM_D8, RT_NONE, RT_NONE}},
            {0xEF, {IN_RST, AM_IMP, RT_NONE, RT_NONE}}, // RST 28H

            // 0xF0 - 0xFF
            {0xF0, {IN_LDH, AM_R_A8, RT_A, RT_NONE}},
            {0xF1, {IN_POP, AM_R, RT_AF, RT_NONE}},
            {0xF2, {IN_LD, AM_R_MR, RT_A, RT_C}}, // LD A,(C)
            {0xF3, {IN_DI, AM_IMP, RT_NONE, RT_NONE}},
            {0xF4, {IN_ERR, AM_IMP, RT_NONE, RT_NONE}}, // Invalid
            {0xF5, {IN_PUSH, AM_R, RT_AF, RT_NONE}},
            {0xF6, {IN_OR, AM_D8, RT_NONE, RT_NONE}},
            {0xF7, {IN_RST, AM_IMP, RT_NONE, RT_NONE}}, // RST 30H
            {0xF8, {IN_LD, AM_HL_SPR, RT_HL, RT_SP}},
            {0xF9, {IN_LD, AM_R_R, RT_SP, RT_HL}},
            {0xFA, {IN_LD, AM_R_A16, RT_A, RT_NONE}},
            {0xFB, {IN_EI, AM_IMP, RT_NONE, RT_NONE}},
            {0xFC, {IN_ERR, AM_IMP, RT_NONE, RT_NONE}}, // Invalid
            {0xFD, {IN_ERR, AM_IMP, RT_NONE, RT_NONE}}, // Invalid
            {0xFE, {IN_CP, AM_D8, RT_NONE, RT_NONE}},
            {0xFF, {IN_RST, AM_IMP, RT_NONE, RT_NONE}}  // RST 38H
    };
}

CPU::OpcodeInfo CPU::OpcodeToInstruction(uint8_t opcode) const
{
    auto it = this->OpcodeTable.find(opcode);
    if (it != this->OpcodeTable.end()) {
		return it->second; // donne l'instruction correspondante | it->first donne le clé (l'opcode)
    }

    return { IN_ERR, AM_IMP, RT_NONE, RT_NONE }; // Return an error instruction if not found
}

uint8_t CPU::readRegister8bit(RegType RT)
{
	uint8_t value = 0;
	switch (RT) {
	case RT_A: value = reg.A; break;
	case RT_B: value = reg.B; break;
	case RT_C: value = reg.C; break;
	case RT_D: value = reg.D; break;
	case RT_E: value = reg.E; break;
	case RT_H: value = reg.H; break;
	case RT_L: value = reg.L; break;
	default:
		std::cerr << "Invalid register read: " << static_cast<int>(RT) << "\n";
		exit(1);
	}
	return value;
}
uint16_t CPU::readRegister16bit(RegType RT)
{
	uint16_t value = 0;
	switch (RT) {
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

void CPU::setFlags(char z, char n, char h, char c)
{
    if (z != -1)
    {
		reg.F = (reg.F & 0x7F) | (z << 7); // Set Z flag
    }

    if (n != -1)
    {
		reg.F = (reg.F & 0xBF) | (n << 6); // Set N flag
    }

    if (h != -1)
    {
		reg.F = (reg.F & 0xEF) | (h << 5); // Set H flag
    }

    if (c != -1)
    {
		reg.F = (reg.F & 0xF7) | (c << 4); // Set C flag
    }
}

void CPU::executeOpcode(uint8_t opcode, Bus& bus)
{
	destMem = false;
    currentInstruction = OpcodeToInstruction(opcode);
	fetchData(currentInstruction, bus);
}

void CPU::fetchData(OpcodeInfo curInstr, Bus& bus)
{
	switch (curInstr.AM)
	{
	case AM_R_D16:
        uint16_t lo = bus.read(reg.PC++);
        uint16_t hi = bus.read(reg.PC++);

		fetchDataVal = (hi << 8) | lo;
		break;
	case AM_R_D8:
		fetchDataVal = bus.read(reg.PC++);
		break;
	case AM_MR_R:
        fetchDataVal = readRegister8bit(currentInstruction.RT2);
        destMem = readRegister16bit(currentInstruction.RT1);
        destIsMem = true;

        if (currentInstruction.RT1 == RT_C) {
            destMem |= 0xFF00;
        }
		break;
	case AM_R_MR:
		fetchDataVal = bus.read(reg.PC++);
		break;
	case AM_HLI_R:
		// Handle HLI operation
		break;
	case AM_HLD_R:
		// Handle HLD operation
		break;
	case AM_R_A8:
		// Handle A8 addressing mode
		break;
	case AM_A8_R:
		// Handle A8 addressing mode for writing to register
		break;
	default:
		std::cerr << "Unsupported addressing mode: " << static_cast<int>(curInstr.AM) << "\n";
		exit(1);
	}
}