#include <iostream>
#include <conio.h>
#include <stdio.h>
#include "variables.h"

using namespace std;

static INST_Table inst[30];

//初始化状态表
void init_statusTable()
{
	//初始化表2
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			function_status[i][j] = 0;
		}
	}

	//初始化表1
	for (int i = 0; i < 30; i++)
		register_status[i] = 0;
}

//指令输入
int input_inst()
{
	int count = 0;
	char op[10] = "", r1[5] = "", r2[5] = "", r3[5] = "";
	cout << "请输入指令(以end结束):" << endl;
	cin >> op;
	while (strcmp(op, "end") != 0)
	{
		cin >> r1 >> r2 >> r3;
		inst[++count].Input(op, r1, r2, r3);
		cout << "请继续输入指令(以end结束):" << endl;
		cin >> op;
	}
	return count;
}

int input_inst_exa()
{
	const int count = 6;
	char instr[count][50] = {
		"LD  F6  0  R2",
		"LD  F2  0  R3",
		"MULTD F1 F2 F4",
		"SUBD  F8 F6 F2",
		"DIVD  F10 F1  F6",
		"ADDD  F6  F8  F2"
	};

	cout << "指令集为：" << endl;
	char op[10] = "", r1[5] = "", r2[5] = "", r3[5] = "";
	for (int i = 0; i < count; i++)
	{
		char tmp_instr[50] = { 0 };
		strcpy(tmp_instr, instr[i]);
		cout << tmp_instr << endl;
		const char *split = " ";
		char *p;
		p = strtok(tmp_instr, split);
		strcpy(op, p);
		p = strtok(NULL, split);
		strcpy(r1, p);
		p = strtok(NULL, split);
		strcpy(r2, p);
		p = strtok(NULL, split);
		strcpy(r3, p);
		inst[i+1].Input(op, r1, r2, r3);

	}
	return count;
}

//该函数可以修改表2和表3的状态
void alter_table(int fu, INSTRUCTION *ins, int i)
{
	//更改表2
	function_status[fu][BUSY] = yes;
	function_status[fu][OP] = ins->name;
	function_status[fu][FI] = ins->dest;
	function_status[fu][FJ] = ins->sr1;
	function_status[fu][FK] = ins->sr2;
	//若Fj是Rj则可以直接读取，若Fj为0即没有j操作数，故此时Rj=yes
	if (function_status[fu][FJ] > 10 || function_status[fu][FJ] == 0)
		function_status[fu][RJ] = yes;
	if (function_status[fu][FK] > 10 || function_status[fu][FK] == 0)
		function_status[fu][RK] = yes;
	//若Fj与之前运行中的指令目的寄存器都不相关，则也可以直接读取
	int flag = 1;
	for (int j = 1; j < i; j++)
	{
		if (function_status[inst[j].getFU()][BUSY] == 1)
		{
			if (inst[j].ins->dest == ins->sr1)
			{
				flag = 0;
				function_status[fu][QJ] = inst[j].getFU();
				break;
			}
		}
	}
	if (flag)
		function_status[fu][RJ] = yes;
	flag = 1;
	for (int j = 1; j < i; j++)
	{
		if (function_status[inst[j].getFU()][BUSY] == 1)
		{
			if (inst[j].ins->dest == ins->sr2)
			{
				flag = 0;
				function_status[fu][QK] = inst[j].getFU();
				break;
			}
		}
	}
	if (flag)
		function_status[fu][RK] = yes;

	//更改表3
	register_status[ins->dest] = fu;

}

//指令各个阶段的判断以及状态表的修改
void inst_issue(int i, int count)
{
	//检查每一条之前指令的RAW冒险，即i指令的IS必须比前面的指令的WB大(也就是说，相关时，i指令不能在前一条指令WB之前IS)
	for (int j = 1; j <= i; j++)
	{
		if ((inst[j].ins->dest == inst[i].ins->sr1) || (inst[j].ins->dest == inst[i].ins->sr2))
		{
			if (Cycle - inst[j].instruction_status[WB] <= 0)
				return;
		}
	}

	int p = i - 1;
	switch (inst[i].ins->name)
	{
	case LD:
		if ((function_status[INTEGER][BUSY] == no) && (inst[i].instruction_status[IS] == 0))
		{
			if ((p > 0 && (inst[p].instruction_status[IS] != 0) && (Cycle > inst[p].issue) && (Cycle != inst[p].writeback)) || p == 0)
			{
				inst[i].setFU(INTEGER);
				//检查之前运行指令的WAW冒险
				for (int k = 1; k <= count; k++)
				{
					if (inst[k].exc != 0)
					{
						if (inst[k].ins->dest == inst[i].ins->dest)
							return;
					}
				}
				//修改状态表2和表3
				alter_table(INTEGER, inst[i].ins, i);
				break;
			}
			else
				return;
		}
		else
			return;
	case MULTD:
		if ((function_status[MULT1][BUSY] == no) && (inst[i].instruction_status[IS] == 0))
		{
			if ((p > 0 && (inst[p].instruction_status[IS] != 0) && (Cycle > inst[p].issue) && (Cycle != inst[p].writeback)) || p == 0)
			{
				inst[i].setFU(MULT1);
				//检查之前运行指令的WAW冒险
				for (int k = 1; k <= count; k++)
				{
					if (inst[k].exc != 0)
					{
						if (inst[k].ins->dest == inst[i].ins->dest)
							return;
					}
				}

				//修改状态表2和表3
				alter_table(MULT1, inst[i].ins, i);
				break;
			}
			else
				return;
		}
		else if ((function_status[MULT2][BUSY] == no) && (inst[i].instruction_status[IS] == 0))
		{
			if ((p > 0 && (inst[p].instruction_status[IS] != 0) && (Cycle > inst[p].issue) && (Cycle != inst[p].writeback)) || p == 0)
			{
				inst[i].setFU(MULT2);
				//检查之前运行指令的WAW冒险
				for (int k = 1; k <= count; k++)
				{
					if (inst[k].exc != 0)
					{
						if (inst[k].ins->dest == inst[i].ins->dest)
							return;
					}
				}
				
				//修改状态表2和表3
				alter_table(MULT2, inst[i].ins, i);
				break;
			}
			else
				return;
		}
		else
			return;
	case SUBD:
		if ((function_status[ADD][BUSY] == no) && (inst[i].instruction_status[IS] == 0))
		{
			if (p > 0 && (inst[p].instruction_status[IS] != 0) && (Cycle > inst[p].issue) && (Cycle != inst[p].writeback) || p == 0)
			{
				inst[i].setFU(ADD);
				//检查之前运行指令的WAW冒险
				for (int k = 1; k <= count; k++)
				{
					if (inst[k].exc != 0)
					{
						if (inst[k].ins->dest == inst[i].ins->dest)
							return;
					}
				}

				//修改状态表2和表3
				alter_table(ADD, inst[i].ins, i);
				break;
			}
			else
				return;
		}
		else
			return;
	case DIVD:
		if ((function_status[DIV][BUSY] == no) && (inst[i].instruction_status[IS] == 0))
		{
			if ((p > 0 && (inst[p].instruction_status[IS] != 0) && (Cycle > inst[p].issue) && (Cycle != inst[p].writeback)) || p == 0)
			{
				inst[i].setFU(DIV);
				//检查之前运行指令的WAW冒险
				for (int k = 1; k <= count; k++)
				{
					if (inst[k].exc != 0)
					{
						if (inst[k].ins->dest == inst[i].ins->dest)
							return;
					}
				}

				//修改状态表2和表3
				alter_table(DIV, inst[i].ins, i);
				break;
			}
			else
				return;
		}
		else
			return;
	case ADDD:
		if ((function_status[ADD][BUSY] == no) && (inst[i].instruction_status[IS] == 0))
		{
			if ((p > 0 && (inst[p].instruction_status[IS] != 0) && (Cycle > inst[p].issue) && (Cycle != inst[p].writeback)) || p == 0)
			{
				inst[i].setFU(ADD);
				//检查之前运行指令的WAW冒险
				for (int k = 1; k <= count; k++)
				{
					if (inst[k].exc != 0)
					{
						if (inst[k].ins->dest == inst[i].ins->dest)
							return;
					}
				}

				//修改状态表2和表3
				alter_table(ADD, inst[i].ins, i);
				break;
			}
			else
				return;
		}
		else
			return;
	}

	//修改状态表1
	inst[i].setIS(Cycle);
	inst[i].setSTATUS(IS, Cycle);
}

void inst_readop(int i)
{
	int tmp = inst[i].getFU();
	int Rj = function_status[tmp][RJ];
	int Rk = function_status[tmp][RK];


	for (int j = 1; j <= i; j++)
	{
		//对于之前正在执行的指令，检查RAW冒险，也就是说i条指令Rj和Rk没准备好的时候
		if (inst[j].exc != 0)
		{
			if ((inst[j].ins->dest == inst[i].ins->sr1) || (inst[j].ins->dest == inst[i].ins->sr2) || Rj == 0 || Rk == 0)
			{
				return;
			}
		}
		//对于执行完成的指令，则可以按照相关条件提供源操作数，即更改表2的Rj和Rk
		else
		{
			if ((inst[j].ins->dest == inst[i].ins->sr1) || (inst[j].ins->dest == inst[i].ins->sr2))
			{
				if (inst[j].ins->dest == inst[i].ins->sr1)
				{
					function_status[tmp][QJ] = 0;
					function_status[tmp][RJ] = yes;
				}
				if (inst[j].ins->dest == inst[i].ins->sr2)
				{
					function_status[tmp][QK] = 0;
					function_status[tmp][RK] = yes;
				}
			}
		}
		//检查每一条之前执行完指令的RAW冒险，即i指令的RD必须比前面的指令的WB大(也就是说，相关时，i指令不能在前一条指令WB之前RD)
		if ((inst[j].ins->dest == inst[i].ins->sr1) || (inst[j].ins->dest == inst[i].ins->sr2) || inst[j].exc == 0)
		{
			if (Cycle - inst[j].instruction_status[WB] <= 0)
				return;
		}
	}
	//修改状态表1
	inst[i].setRD(Cycle);
	inst[i].setSTATUS(RD, Cycle);
	inst[i].setEXC(1);					//指令开始执行
}

void inst_execution(int i)
{
	//修改状态表1
	inst[i].setEX(Cycle);
	inst[i].setSTATUS(EX, Cycle);
}

void inst_writeback(int i)
{
	//检查WAR冒险
	for (int j = 1; j <= i; j++)
	{
		if (inst[j].exc != 0)
		{
			if ((inst[j].ins->sr1 == inst[i].ins->dest) || (inst[j].ins->sr2 == inst[i].ins->dest))
				return;
		}
	}
	//修改状态表1
	inst[i].setWB(Cycle);
	inst[i].setSTATUS(WB, Cycle);
}

//执行模块
void execution(int count, int label = 0)
{
	int flag = 1;

	//时间驱动
	while (flag)
	{
		//每个cycle都对每条指令进行状态更新
		for (int i = 1; i <= count; i++)
		{
			if (inst[i].instruction_status[IS] == 0)
			{
				inst_issue(i, count);
			}
			else
			{
				if (inst[i].instruction_status[RD] == 0)
				{
					inst_readop(i);
				}
				else
				{
					if (inst[i].instruction_status[EX] == 0 && Cycle - inst[i].readop == inst[i].ins->cycle)
					{
						inst_execution(i);
					}
					else
					{
						if (inst[i].instruction_status[EX] != 0  && inst[i].instruction_status[WB] == 0)
						{
							inst_writeback(i);
						}
					}
				}
			}


			//对于执行完的指令，必须对其exc位和表2对应的Busy位归位置0
			if (inst[i].instruction_status[WB] != 0 && inst[i].exc == 1)
			{
				int tmp_fu = inst[i].getFU();
				function_status[tmp_fu][BUSY] = no;
				inst[i].setEXC(0);
				inst[i].setFU(NONE);
				//执行完指令的目的寄存器表项清零
				//register_status[inst[i].ins->dest] = 0;
			}
		}



		//若所有的指令均写回，则终止
		int cnt;
		for (cnt = 1; cnt <= count; cnt++)
		{
			if (inst[cnt].instruction_status[WB] == 0)
			{
				flag = 1;
				break;
			}
		}
		
		if (cnt > count)
			flag = 0;

		//在label周期停止以输出label周期的状态表
		if (label != 0)
		{
			if (Cycle == label)
				return;
		}

		Cycle++;
	}
}

//状态表输出
void output_statusTable(int count)
{
	cout << '\n';

	cout << "指令状态表："<< endl;
	cout << "******************************************************************************" << endl;
	for (int i = 1; i <= count; i++)
	{
		for (int j = 1; j < 5; j++)
		{
			if (inst[i].instruction_status[j] == 0)
				cout << " " << '\t';
			else
				cout << inst[i].instruction_status[j] << '\t';
		}
		cout << '\n';
	}
	cout << "******************************************************************************" << endl << endl;
	cout << "功能单元状态表：" << endl;
	cout << "******************************************************************************" << endl;
	cout << "BUSY" << '\t' << "OP" << '\t' << "Fi" << '\t' << "Fj" << '\t' << "Fk" << '\t' << "Qj" << '\t' << "Qk" << '\t' << "Rj" << '\t' << "Rk" << '\t' << '\n';
	for (int i = 1; i <= 5; i++)
	{
		for (int j = 1; j <= 9; j++)
		{
			switch (j)
			{
			case 1:
				if (function_status[i][j])
					cout << "yes" << '\t';
				else
					cout << "no" << '\t';
				break;
			case 2:
				switch (function_status[i][j])
				{
				case 0:
					cout << "no" << '\t';
					break;
				case LD:
					cout << "LD" << '\t';
					break;
				case MULTD:
					cout << "MULTD" << '\t';
					break;
				case DIVD:
					cout << "DIVD" << '\t';
					break;
				case ADDD:
					cout << "ADDD" << '\t';
					break;
				case SUBD:
					cout << "SUBD" << '\t';
					break;
				}
				break;
			case 6: case 7:
				if (function_status[i][j] == 0)
					cout << "0" << '\t';
				else
				{
					switch (function_status[i][j])
					{
					case INTEGER:
						cout << "INTEGER" << '\t';
						break;
					case MULT1:
						cout << "MULT1" << '\t';
						break;
					case MULT2:
						cout << "MULT2" << '\t';
						break;
					case ADD:
						cout << "ADD" << '\t';
						break;
					case DIV:
						cout << "DIV" << '\t';
						break;
					}
				}					
				break;
			case 8:case 9:
				if (function_status[i][j])
					cout << "yes" << '\t';
				else
					cout << "no" << '\t';
				break;
			default:
				if (function_status[i][j] <= 10)
					cout << "F" << function_status[i][j] << '\t';
				else
					cout << "R" << function_status[i][j] - 10 << '\t';
				break;

			}
		}
		cout << '\n';
	}
	cout << "******************************************************************************" << endl << endl;

	cout << "寄存器状态表：" << endl;
	cout << "******************************************************************************" << endl;
	for (int i = 0; i <= 29; i++)
	{
		if (register_status[i] != 0)
		{
			switch (register_status[i])
			{
			case INTEGER:
				cout << "F" << i << ":INTEGER" << '\t';
				break;
			case MULT1:
				cout << "F" << i << ":MULT1" << '\t';
				break;
			case MULT2:
				cout << "F" << i << ":MULT2" << '\t';
				break;
			case ADD:
				cout << "F" << i << ":ADD" << '\t';
				break;
			case DIV:
				cout << "F" << i << ":DIV" << '\t';
				break;

			}
		}
		else 
			cout << "F" << i << ":0" << '\t';

	}
	cout << '\n';
	cout << "******************************************************************************" << endl << endl;
}

int main()
{
	init_statusTable();
	//指令模式
	int count = input_inst_exa();
	execution(count);
	output_statusTable(count);

	system("pause");
	return 0;
}