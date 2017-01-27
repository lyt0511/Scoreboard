#pragma once
#include <stdio.h>
#include <string.h>

//
enum REGISTER {F1=1, F2, F3, F4, F5, F6, F7, F8, F9, F10, R1, R2, R3, R4};

enum OP {LD=1, MULTD, DIVD, ADDD, SUBD};

enum STAGE {IS=1, RD, EX, WB};

enum FUNCTION_UNIT {NONE, INTEGER=1, MULT1, MULT2, ADD, DIV};

enum LABEL {BUSY=1, OP, FI, FJ, FK, QJ, QK, RJ, RK};

#define no 0

#define yes 1

extern int function_status[6][10];

extern int register_status[30];

extern int Cycle;

struct INSTRUCTION
{
	int name;
	int cycle;
	int dest;
	int sr1;
	int sr2;
};

class INST_Table
{
public:
	INSTRUCTION *ins;

	int instruction_status[5];
	int fu;
	int exc;
	int issue;
	int readop;
	int exeution;
	int writeback;

	INST_Table()
	{
		this->ins = new INSTRUCTION;

		this->instruction_status[0] = 0;
		this->instruction_status[1] = 0;
		this->instruction_status[2] = 0;
		this->instruction_status[3] = 0;
		this->instruction_status[4] = 0;

		this->fu = 0;
		this->exc = 0;
		this->issue = 0;
		this->readop = 0;
		this->exeution = 0;
		this->writeback = 0;
	}

	void setSTATUS(int i, int val);
	void setFU(FUNCTION_UNIT val);
	void setEXC(int val);
	void setIS(int val);
	void setRD(int val);
	void setEX(int val);
	void setWB(int val);
	int getFU();
	void Input(char name[], char rd[], char rs1[], char rs2[]);

	~INST_Table()
	{
		delete ins;
	}
};
