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

//J type
void J_Type(unsigned int instWord) {
	unsigned int rd, opcode;
	unsigned int J_imm1, J_imm2, J_imm = 0;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;


	J_imm1 = ((instWord >> 12) & 0x0007FFFF) | (((instWord >> 31) ? 0xFFF80000 : 0x0));//imm[20|10:1|11|19:12]
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
	if (rd != 0)
		reg[rd - 1] = pc;
	pc = (pc - 4) + J_imm;
}


int main() {

	unsigned int instWord = 0x0070016F;
	J_Type(instWord);
}