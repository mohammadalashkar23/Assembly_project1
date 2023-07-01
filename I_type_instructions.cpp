// I_type_instructions.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cmath>
using namespace std;
unsigned int pc;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[31];
const unsigned int reg0 = 0;
void I_type(unsigned int instWord);
int main()
{
	unsigned int instWord;
	cin >> instWord;
	I_type(instWord);
	cout << 0 << " " << reg0 << endl;

	for (int i = 0; i < 31; i++)
	{
		cout << i+1 << " " << reg[i] << endl;
	}
}
void I_type(unsigned int instWord)
{
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;



	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	
	// — inst[31] — inst[30:25] inst[24:21] inst[20]
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

	unsigned shamt;
	shamt = I_imm & 0x01F;
	funct7 = (I_imm>>5) & 0x07F;
	
	if (opcode == 0x13) {	// I instructions
		switch (funct3) {
		case 0:	cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				reg[rd - 1] = I_imm;
			}
			else
			{
				reg[rd - 1] = reg[rs1-1]+ I_imm;
			}
			break;
		case 1:	cout << "\tSLLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << shamt << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				reg[rd - 1] = 0;
			}
			else
			{
				reg[rd - 1] = reg[rs1 - 1] *(pow(2,shamt));
			}
			break;
		case 2:	cout << "\tSLTI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				if (((int)reg0) < ((int)I_imm))
				{
					reg[rd - 1] = 1;
				}
				else 
				{ 
					reg[rd - 1] = 0;
				}
			}
			else
			{

				if (((int)reg[rs1-1]) < ((int)I_imm))
				{
					reg[rd - 1] = 1;
				}
				else
				{
					reg[rd - 1] = 0;
				}
			}
			break;
		case 3:	cout << "\tSLTIU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				if ((reg0) < (I_imm))
				{
					reg[rd - 1] = 1;
				}
				else
				{
					reg[rd - 1] = 0;
				}
			}
			else
			{

				if ((reg[rs1 - 1]) < (I_imm))
				{
					reg[rd - 1] = 1;
				}
				else
				{
					reg[rd - 1] = 0;
				}
			}
			break;
		case 4:	cout << "\tXORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				reg[rd - 1] = I_imm;
			}
			else
			{
				reg[rd - 1] = reg[rs1 - 1] ^ I_imm;
			}
			break;
		case 5:	
			switch (funct7) {
			case 0:	cout << "\tSRLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << shamt << "\n";
				if (rd == 0)
				{

				}
				else if (rs1 == 0)
				{
					reg[rd - 1] = 0;
				}
				else
				{
					reg[rd - 1] = reg[rs1 - 1] >> shamt;
				}
				break;
			case 32:	cout << "\tSRAI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << shamt << "\n";
				if (rd == 0)
				{

				}
				else if (rs1 == 0)
				{
					reg[rd - 1] = 0;
				}
				else
				{
					int tempreg = reg[rs1 - 1];
					reg[rd - 1] = tempreg >> shamt;
				}
				break;
			default:
				cout << "\tUnkown I Instruction \n";
			}
			break;
		case 6:	cout << "\tORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				reg[rd - 1] = I_imm;
			}
			else
			{
				reg[rd - 1] = reg[rs1 - 1] | I_imm;
			}
			break;
		case 7:	cout << "\tANDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				reg[rd - 1] = 0;
			}
			else
			{
				reg[rd - 1] = reg[rs1 - 1] & I_imm;
			}
			break;
		default:
			cout << "\tUnkown I Instruction \n";
		}
	}
	else {
		switch (funct3) {
		case 0:	cout << "\tLB\tx" << rd << ", "  << (int)I_imm << " (x" << rs1 << ')' << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				int temp = I_imm;
				int extending;
				if ((temp >= 0) && (temp <= sizeof(memory) - 1))
				{
					reg[rd - 1] = 0;
					reg[rd - 1] = memory[temp];
					extending = reg[rd - 1] << 24;
					extending = extending >> 24;
					reg[rd - 1] = extending;

				}
				else
				{
					cout << "\terror out of memory range\t" << endl;
				}
			}
			else
			{
				int temp = I_imm + reg[rs1 - 1];
				int extending;
				if ((temp >= 0) && (temp <= sizeof(memory) - 1))
				{
					reg[rd - 1] = 0;
					reg[rd - 1] = memory[temp];
					extending = reg[rd - 1] << 24;
					extending = extending >> 24;
					reg[rd - 1] = extending;

				}
				else
				{
					cout << "\terror out of memory range\t" << endl;
				}
			}
			break;
		case 1:	cout << "\tLH\tx" << rd << ", "  << (int)I_imm << " (x" << rs1 << ')' << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				int temp = I_imm;
				int extending;
				if ((temp >= 0) && (temp + 1 <= sizeof(memory) - 1))
				{
					reg[rd - 1] = 0;
					reg[rd - 1] = memory[temp+1];
					reg[rd - 1] = reg[rd - 1] << 8;
					reg[rd - 1] = reg[rd - 1] | memory[temp ];
					extending = reg[rd - 1] << 16;
					extending = extending >> 16;
					reg[rd - 1] = extending;

				}
				else
				{
					cout << "\terror out of memory range\t" << endl;
				}
			}
			else
			{
				int temp = I_imm + reg[rs1 - 1];
				int extending;
				if ((temp >= 0) && (temp + 1 <= sizeof(memory) - 1))
				{
					reg[rd - 1] = 0;
					reg[rd - 1] = memory[temp + 1];
					reg[rd - 1] = reg[rd - 1] << 8;
					reg[rd - 1] = reg[rd - 1] | memory[temp];
					extending = reg[rd - 1] << 16;
					extending = extending >> 16;
					reg[rd - 1] = extending;

				}
				else
				{
					cout << "\terror out of memory range\t" << endl;
				}
			}
			break;
		case 2:	cout << "\tLW\tx" << rd << ", "  << (int)I_imm << " (x" << rs1 << ')' << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				int temp = I_imm;
				if ((temp >= 0) && (temp + 3 <= sizeof(memory)-1))
				{ 
					reg[rd - 1] = 0;
					reg[rd - 1] = memory[temp +3];
					for (int i = 3; i > 0; i--)
					{
						reg[rd - 1] = reg[rd - 1] << 8;
						reg[rd - 1] = reg[rd - 1] | memory[temp + i-1];
					}
				}
				else
				{
					cout <<"\terror out of memory range\t" << endl;
				}
			}
			else
			{
				int temp = I_imm+reg[rs1-1];
				if ((temp >= 0) && (temp + 3 <= sizeof(memory)-1))
				{
					reg[rd - 1] = 0;
					reg[rd - 1] = memory[temp + 3];
					for (int i = 3; i > 0; i--)
					{
						reg[rd - 1] = reg[rd - 1] << 8;
						reg[rd - 1] = reg[rd - 1] | memory[temp + i - 1];
					}
				}
				else
				{
					cout << "\terror out of memory range\t" << endl;
				}
			}
			break;
		case 4:	cout << "\tLBU\tx" << rd << ", " << (int)I_imm << " (x" << rs1 << ')' << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				int temp = I_imm;
				if ((temp >= 0) && (temp <= sizeof(memory) - 1))
				{
					reg[rd - 1] = 0;
					reg[rd - 1] = memory[temp];
				}
				else
				{
					cout << "\terror out of memory range\t" << endl;
				}
			}
			else
			{
				int temp = I_imm + reg[rs1 - 1];
				if ((temp >= 0) && (temp  <= sizeof(memory) - 1))
				{
					reg[rd - 1] = 0;
					reg[rd - 1] = memory[temp];
				}
				else
				{
					cout << "\terror out of memory range\t" << endl;
				}
			}
			break;
		case 5:	cout << "\tLHU\tx" << rd << ", " << (int)I_imm << " (x" << rs1 << ')' << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				int temp = I_imm;
				if ((temp >= 0) && (temp + 1 <= sizeof(memory)-1))
				{
					reg[rd - 1] = 0;
					reg[rd - 1] = memory[temp + 1];
					for (int i = 1; i > 0; i--)
					{
						reg[rd - 1] = reg[rd - 1] << 8;
						reg[rd - 1] = reg[rd - 1] | memory[temp + i - 1];
					}
				}
				else
				{
					cout << "\terror out of memory range\t" << endl;
				}
			}
			else
			{
				int temp = I_imm + reg[rs1 - 1];
				if ((temp >= 0) && (temp + 1 <= sizeof(memory)-1))
				{
					reg[rd - 1] = 0;
					reg[rd - 1] = memory[temp + 1];
					for (int i = 1; i > 0; i--)
					{
						reg[rd - 1] = reg[rd - 1] << 8;
						reg[rd - 1] = reg[rd - 1] | memory[temp + i - 1];
					}
				}
				else
				{
					cout << "\terror out of memory range\t" << endl;
				}
			}
			break;
		default:
			cout << "\tUnkown I_load Instruction \n";
		}
	}
}
