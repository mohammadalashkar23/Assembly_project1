// compressed_instructions.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>	
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[31];
const unsigned int reg0 = 0;
using namespace std;
int main()
{
    std::cout << "Hello World!\n";
}
unsigned int decompress2(unsigned short compressed_word)
{
	unsigned int instword;
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int opC, rs2C, rs1C, funct4C;
	unsigned int nzimm_5;
	unsigned int funct3C;
	unsigned int shamt;
	opC = (compressed_word) & 0x00000003;
	rs2C = (compressed_word >> 2) & 0x0000001f;
	rs1C = (compressed_word >> 7) & 0x0000001f;
	funct4C = (compressed_word >> 12) & 0x0000000f;
	nzimm_5 = (compressed_word >> 12) & 0x00000001;
	funct3C = (compressed_word >> 13) & 0x0000000f;
	if (funct3C == 0b000) { // C.slli

		if (nzimm_5 == 0) {	// c.slli
			opcode = 0b0010011;
			funct3 = 0x1;
			
			rd = rs1C;
			rs1 = rs1C;
			shamt = rs2C;
			instword = 0x00000000;
			instword = (instword << 5) | shamt;
			instword = (instword << 5) | rs1;
			instword = (instword << 3) | funct3;
			instword = (instword << 5) | rd;
			instword = (instword << 7) | opcode;
		}
		else 
		{
			cout << "\tUnkown Compressed Instruction \n";
		}


	}
	else {
		cout << "\tUnkown Compressed Instruction \n";

	}

	return instword;

}
