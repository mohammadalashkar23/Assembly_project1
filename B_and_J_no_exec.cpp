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

unsigned int pc = 0;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[31];
const unsigned int reg0 = 0;

//B type
void B_Type(unsigned int instWord)
{
	unsigned int rs1, rs2, funct3, opcode;
	unsigned int B_imm2, B_imm1, B_imm = 0;
	unsigned int target_address;

	opcode = instWord & 0x0000007F;
	B_imm1 = (instWord >> 7) & 0x0000001F;//imm[4:1|11]
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;

	B_imm2 = ((instWord >> 25) & 0x3F) | (((instWord >> 31) ? 0xFFFFFFC0 : 0x0)); //imm[12|10:5]
	B_imm = ((B_imm2 >> 6) << 1) & 0xFFFFFFFF;
	B_imm = B_imm | (B_imm1 & 0x00000001); //imm[12:11]
	B_imm = (B_imm << 6) | (B_imm2 & 0x0000003F); //imm[12:5]

	B_imm = (B_imm << 4) | ((B_imm1 >> 1) & 0x0000000F); //imm[12:1]
	B_imm = B_imm << 1; //imm[12:0]

	target_address = (pc - 4) + B_imm;
	switch (funct3) {
	case 0: cout << "\tBEQ\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
		break;
	case 1: cout << "\tBNE\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
		break;
	case 4: cout << "\tBLT\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
		break;
	case 5: cout << "\tBGE\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
		break;
	case 6: cout << "\tBLTU\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
		break;
	case 7: cout << "\tBGEU\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
		break;
	default:
		cout << "\tUnkown B Instruction \n";
	}

}
//J type
void J_Type(unsigned int instWord) {
	unsigned int rd, opcode;
	unsigned int J_imm1, J_imm2, J_imm = 0;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;


	J_imm1 = ((instWord >> 12) & 0x0007FFFF) | ((instWord >> 31) ? 0xFFF80000 : 0x0);//imm[20|10:1|11|19:12]
	J_imm = J_imm1 & 0xFFF80000; //imm[20]
	J_imm2 = (J_imm1 & 0x000000FF) << 1; //imm[19:12]
	J_imm1 = J_imm1 >> 8; //imm[20|10:1|11]
	J_imm2 = J_imm2 | (J_imm1 & 0x00000001);//imm[19:11]
	J_imm1 = J_imm1 >> 1; //imm[20|10:1]
	J_imm2 = J_imm2 << 10;
	J_imm2 = J_imm2 | (J_imm1 & 0x000003FF); //imm[19:1]

	J_imm = J_imm | J_imm2;
	J_imm = J_imm << 1;
	cout << "\tJAL\tx" << rd << ", " << hex << "0x" << (int)J_imm << "\n";
}