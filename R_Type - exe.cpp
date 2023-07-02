/*
	This is just a skeleton. It DOES NOT implement all the requirements.
	It only recognizes the RV32I "ADD", "SUB" and "ADDI" instructions only.
	It prints "Unkown Instruction" for all other instructions!

	References:
	(1) The risc-v ISA Manual ver. 2.1 @ https://riscv.org/specifications/
	(2) https://github.com/michaeljclark/riscv-meta/blob/master/meta/opcodes
*/

#include <iostream>	
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[31];
const unsigned int reg0 = 0;

//R type
void R_Type(unsigned int instWord)
{
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;

	unsigned int instPC = pc - 4;

	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	funct7 = (instWord >> 25) & 0x0000007F;

	switch(funct3){
		case 0:
			if(funct7 == 32) {
				if(rd==0) {}
				else if(rs1==0){
					if(rs2==0){
						reg[rd-1]=0;
					}
					reg[rd-1]= 0 - reg[rs2-1];
				}
				else if(rs2==0)
				reg[rd-1]= reg[rs1-1] - 0;
				else
				reg[rd-1] = reg[rs1 - 1] - reg[rs2 - 1];
			}
			else if(funct7 == 0) {
				if(rd==0) {}
				else if(rs1==0){
					if(rs2==0){
						reg[rd-1]=0;
					}
					reg[rd-1]= reg[rs2-1];
				}
				else if(rs2==0)
				reg[rd-1]= reg[rs1-1];
				else
				reg[rd-1] = reg[rs1 - 1] + reg[rs2 - 1];
			}
			break;

		case 1:
			{
				if(rd==0) {}
				else if(rs1==0){
					reg[rd-1]= 0;
				}
				else if(rs2==0)
				reg[rd-1]= reg[rs1-1];
				else
				reg[rd-1] = reg[rs1-1] << reg[rs2-1];
			}
			break;

		case 2:
			{
				if(rd==0) {}
				else if(rs1==0){
					if(rs2==0){
						reg[rd-1]=0;
					}
					reg[rd - 1] = ( 0 < static_cast<int>(reg[rs2 - 1])) ? 1 : 0;
				}
				else if(rs2==0)
				reg[rd - 1] = (static_cast<int>(reg[rs1 - 1]) < 0) ? 1 : 0;
				else
				reg[rd - 1] = (static_cast<int>(reg[rs1 - 1]) < static_cast<int>(reg[rs2 - 1])) ? 1 : 0;
			}
			break;

		case 3:
			{
				if(rd==0) {}
				else if(rs1==0){
					if(rs2==0){
						reg[rd-1]=0;
					}
					reg[rd - 1] = ( 0< reg[rs2 - 1]) ? 1 : 0;
				}
				else if(rs2==0)
				reg[rd - 1] = (reg[rs1 - 1] < 0) ? 1 : 0;
				else
				reg[rd - 1] = (reg[rs1 - 1] < reg[rs2 - 1]) ? 1 : 0;
			}
			break;

		case 4:
			{
				if(rd==0) {}
				else if(rs1==0){
					if(rs2==0){
						reg[rd-1]= 0 ^ 0;
					}
					reg[rd-1]= 0 ^ reg[rs2-1];
				}
				else if(rs2==0)
				reg[rd-1]= reg[rs1-1] ^ 0;
				else
				reg[rd-1] = reg[rs1-1] ^ reg[rs2-1];
			}
			break;

		case 5:
			if(funct7 == 32) {
				if(rd==0) {}
				else if(rs1==0){
					reg[rd-1]= 0;
				}
				else if(rs2==0)
				reg[rd-1]= reg[rs1-1];
				else
				reg[rd-1] = static_cast<int>(reg[rs1 - 1]) >> reg[rs2 - 1];
			}
			else if(funct7 == 0) {
				if(rd==0) {}
				else if(rs1==0){
					reg[rd-1]= 0;
				}
				else if(rs2==0)
				reg[rd-1]= reg[rs1-1];
				else
				reg[rd-1] = reg[rs1-1] >> reg[rs2-1];
			}
			break;

		case 6:
			{
				if(rd==0) {}
				else if(rs1==0){
					if(rs2==0){
						reg[rd-1]=0;
					}
					reg[rd-1]= 0 | reg[rs2-1];
				}
				else if(rs2==0)
				reg[rd-1]= reg[rs1-1] | 0;
				else
				reg[rd-1] = reg[rs1-1] | reg[rs2-1];
			}
			break;

		case 7:
			{
				if(rd==0) {}
				else if(rs1==0){
					if(rs2==0){
						reg[rd-1]=0;
					}
					reg[rd-1]= 0 & reg[rs2-1];
				}
				else if(rs2==0)
				reg[rd-1]= reg[rs1-1] & 0;
				else
				reg[rd-1] = reg[rs1-1] & reg[rs2-1];
			}
			break;
	}

}
