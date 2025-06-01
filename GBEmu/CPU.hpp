#ifndef __CPU_HPP__
#define __CPU_HPP__

#include <iostream>
#include <map>
#include "Bus.hpp"
// #include "Stack.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>

class Stack; // Forward declaration

enum RegType
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
};

enum AddrMode
{
	AM_IMP,
	AM_R_D16,
	AM_R_R,
	AM_MR_R,
	AM_R,
	AM_R_D8,
	AM_R_MR,
	AM_R_HLI,
	AM_R_HLD,
	AM_HLI_R,
	AM_HLD_R,
	AM_R_A8,
	AM_A8_R,
	AM_HL_SPR,
	AM_D16,
	AM_D8,
	AM_D16_R,
	AM_MR_D8,
	AM_MR,
	AM_A16_R,
	AM_R_A16
};

enum Instruction
{
	IN_NONE,
	IN_NOP, // OK
	IN_LD, // OK
	IN_INC, // OK
	IN_DEC, // OK
	IN_RLCA, // OK
	IN_ADD, // OK
	IN_RRCA, // OK
	IN_STOP,
	IN_RLA, // OK
	IN_JR, // OK
	IN_RRA, // OK
	IN_DAA, // OK
	IN_CPL, // OK
	IN_SCF, // OK
	IN_CCF, // OK
	IN_HALT,
	IN_ADC, // OK
	IN_SUB, // OK
	IN_SBC, // OK
	IN_AND, // OK
	IN_XOR, // OK
	IN_OR, // OK
	IN_CP, // OK
	IN_POP, // OK
	IN_JP, // OK
	IN_PUSH, // OK
	IN_RET, // OK
	IN_CB,
	IN_CALL, // OK
	IN_RETI, // OK
	IN_LDH,
	IN_DI,
	IN_EI,
	IN_RST, // OK
	IN_ERR,
	//CB instructions...
	IN_RLC,
	IN_RRC,
	IN_RL,
	IN_RR,
	IN_SLA,
	IN_SRA,
	IN_SWAP,
	IN_SRL,
	IN_BIT,
	IN_RES,
	IN_SET
};

enum CT
{
	CT_NONE,
	CT_C,
	CT_NC,
	CT_Z,
	CT_NZ
};

struct OpcodeInfo
{
	Instruction IN;
	AddrMode AM;
	RegType RT1;
	RegType RT2;
	CT condition;
};

struct Register
{
	uint8_t A;
	uint8_t F;
	uint8_t B;
	uint8_t C;
	uint8_t D;
	uint8_t E;
	uint8_t H;
	uint8_t L;

	uint16_t SP;
	uint16_t PC;

	bool IME; // Interrupt Master Enable
};

class CPU {
public:
	CPU();
	~CPU() { delete stack; }  
	bool isHalted() const { return halted; }
	bool isRunning() const { return running; }

	uint16_t readRegister(RegType RT);
	void writeRegister(RegType RT, uint16_t val);

	void setFlags(char z, char n, char h, char c);
	void executeOpcode(uint8_t opcode, Bus& bus);
private:
	Stack* stack;

	Register reg;
	OpcodeInfo currentInstruction;
	uint16_t fetchDataVal;
	uint16_t destMem;
	bool destIsMem;

	bool halted;
	bool running;

	std::map<uint8_t, OpcodeInfo> OpcodeTable;

	OpcodeInfo OpcodeToInstruction(uint8_t opcode) const;
	void fetchData(Bus& bus);
	std::string getInstructionName(Instruction instr) const;
	bool checkCond(CT cond);

	void procLD(Bus& bus);
	void procJP();
	void procINC(Bus& bus);
	void procDEC(Bus& bus);
	void procADD(Bus& bus);
	void procJR();
	void procAND();
	void procOR();
	void procXOR();
	void procCP();
	void procADC();
	void procSUB();
	void procSBC();
	void procPOP(Bus& bus);
	void procPUSH(Bus& bus);
	void procCALL(Bus& bus);
	void procRET(Bus& bus);
	void procRST(Bus& bus);
	void procSCF();
	void procCCF();
	void procRETI(Bus& bus);
	void procRLCA();
	void procRRCA();
	void procRRA();
	void procDAA();
	void procCPL();
	void procRLA();
	void procLDH(Bus& bus);
	void procDI();
	void procEI();

	void dbgUpdate(Bus& bus);
	void dbgPrint();
	int msglgth;
	char msg[1024];
};

#endif