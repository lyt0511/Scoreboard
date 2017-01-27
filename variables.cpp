#include "variables.h"

int function_status[6][10];
int register_status[30];

int Cycle = 1;				//时钟周期，用来确定指令执行到哪了

void INST_Table::Input(char name[], char rd[], char rs1[], char rs2[])
{
	//取指令名

	char op1[5] = "LD", op2[7] = "MULTD", op3[5] = "DIVD", op4[5] = "ADDD", op5[5] = "SUBD";

	if (!strcmp(op1, name))
	{
		ins->name = LD;
		ins->cycle = 1;
	}

	if (!strcmp(op2, name))
	{
		ins->name = MULTD;
		ins->cycle = 10;
	}

	if (!strcmp(op3, name))
	{
		ins->name = DIVD;
		ins->cycle = 40;
	}

	if (!strcmp(op4, name))
	{
		ins->name = ADDD;
		ins->cycle = 2;
	}

	if (!strcmp(op5, name))
	{
		ins->name = SUBD;
		ins->cycle = 2;
	}

	//取目的寄存器
	char rd1[5] = "F1", rd2[5] = "F2", rd3[5] = "F3", rd4[5] = "F4", rd5[5] = "F5", rd6[5] = "F6", rd7[5] = "F7", rd8[5] = "F8",
		rd9[5] = "F9", rd10[5] = "F10", rd11[5] = "R1", rd12[5] = "R2", rd13[5] = "R3", rd14[5] = "R4";

	if (!strcmp(rd1, rd))
		ins->dest = F1;

	if (!strcmp(rd2, rd))
		ins->dest = F2;

	if (!strcmp(rd3, rd))
		ins->dest = F3;

	if (!strcmp(rd4, rd))
		ins->dest = F4;

	if (!strcmp(rd5, rd))
		ins->dest = F5;

	if (!strcmp(rd6, rd))
		ins->dest = F6;

	if (!strcmp(rd7, rd))
		ins->dest = F7;

	if (!strcmp(rd8, rd))
		ins->dest = F8;

	if (!strcmp(rd9, rd))
		ins->dest = F9;

	if (!strcmp(rd10, rd))
		ins->dest = F10;

	if (!strcmp(rd11, rd))
		ins->dest = R1;

	if (!strcmp(rd12, rd))
		ins->dest = R2;

	if (!strcmp(rd13, rd))
		ins->dest = R3;

	if (!strcmp(rd14, rd))
		ins->dest = R4;

	//取源寄存器
	if (!strcmp(rd1, rs1))
		ins->sr1 = F1;

	if (!strcmp(rd2, rs1))
		ins->sr1 = F2;

	if (!strcmp(rd3, rs1))
		ins->sr1 = F3;

	if (!strcmp(rd4, rs1))
		ins->sr1 = F4;

	if (!strcmp(rd5, rs1))
		ins->sr1 = F5;

	if (!strcmp(rd6, rs1))
		ins->sr1 = F6;

	if (!strcmp(rd7, rs1))
		ins->sr1 = F7;

	if (!strcmp(rd8, rs1))
		ins->sr1 = F8;

	if (!strcmp(rd9, rs1))
		ins->sr1 = F9;

	if (!strcmp(rd10, rs1))
		ins->sr1 = F10;

	if (!strcmp(rd11, rs1))
		ins->sr1 = R1;

	if (!strcmp(rd12, rs1))
		ins->sr1 = R2;

	if (!strcmp(rd13, rs1))
		ins->sr1 = R3;

	if (!strcmp(rd14, rs1))
		ins->sr1 = R4;

	if (!strcmp(rd1, rs2))
		ins->sr2 = F1;

	if (!strcmp(rd2, rs2))
		ins->sr2 = F2;

	if (!strcmp(rd3, rs2))
		ins->sr2 = F3;

	if (!strcmp(rd4, rs2))
		ins->sr2 = F4;

	if (!strcmp(rd5, rs2))
		ins->sr2 = F5;

	if (!strcmp(rd6, rs2))
		ins->sr2 = F6;

	if (!strcmp(rd7, rs2))
		ins->sr2 = F7;

	if (!strcmp(rd8, rs2))
		ins->sr2 = F8;

	if (!strcmp(rd9, rs2))
		ins->sr2 = F9;

	if (!strcmp(rd10, rs2))
		ins->sr2 = F10;

	if (!strcmp(rd11, rs2))
		ins->sr2 = R1;

	if (!strcmp(rd12, rs2))
		ins->sr2 = R2;

	if (!strcmp(rd13, rs2))
		ins->sr2 = R3;

	if (!strcmp(rd14, rs2))
		ins->sr2 = R4;

	//源寄存器为空的时候
	if (!strcmp("0", rs1))
		ins->sr1 = 0;

	if (!strcmp("0", rs2))
		ins->sr2 = 0;
}	

void INST_Table::setFU(FUNCTION_UNIT f)
{
	this->fu = f;
}

void INST_Table::setSTATUS(int i, int val)
{
	this->instruction_status[i] = val;
}

void INST_Table::setEXC(int val)
{
	this->exc = val;
}

void INST_Table::setIS(int val)
{
	this->issue = val;
}

void INST_Table::setRD(int val)
{
	this->readop = val;
}

void INST_Table::setEX(int val)
{
	this->exeution = val;
}

void INST_Table::setWB(int val)
{
	this->writeback = val;
}

int INST_Table::getFU()
{
	return this->fu;
}