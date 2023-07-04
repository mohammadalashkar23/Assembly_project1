#include <iostream>	
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <bitset>

using namespace std;

unsigned int pc;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[31];
const unsigned int reg0 = 0;

//compressed functions with opcpde '10'
unsigned int decompress0(unsigned short compressed_word)
{
	unsigned int instword;
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int Imm1, Imm2, Imm3, Imm;
	unsigned int opC, rdC, rs1C, rs2C, funct3C;
	unsigned int nzuimm;

	opC = (compressed_word) & 0x00000003;
	funct3C = (compressed_word >> 13) & 0x00000007;

	if (funct3C == 0b010) { //Lw
	//change in rd to rs2 in sw
		rdC = (compressed_word >> 2) & 0x00000007;
		Imm1 = (compressed_word >> 6) & 0x1;   //the second bit in the Imm1
		Imm2 = (compressed_word >> 10) & 0x7;  //get the [3:5] bits
		Imm3 = (compressed_word >> 5) & 0x1; //get the 6 bit
		Imm = (Imm1 << 2) | (Imm2 << 3) | (Imm3 << 6);
		rs1C = (compressed_word >> 7) & 0x00000007;

		opcode = 0b0000011;
		funct3 = 0x2;
		rd = rdC;
		rs1 = rs1C;
		instword = Imm;
		instword = (instword << 5) + rs1;
		instword = (instword << 3) + funct3;
		instword = (instword << 5) + rd;
		instword = (instword << 7) + opcode;
	}
	else if (funct3C == 0b110) { //sw
		rs2C = (compressed_word >> 2) & 0x00000007;
		Imm1 = (compressed_word >> 6) & 0x1;   //the second bit in the Imm1
		Imm2 = (compressed_word >> 10) & 0x7;  //get the [3:5] bits
		Imm3 = (compressed_word >> 5) & 0x1; //get the 6 bit
		Imm = (Imm1 << 2) | (Imm2 << 3) | (Imm3 << 6);
		//	cout << Imm <<endl;

		rs1C = (compressed_word >> 7) & 0x00000007;
		opcode = 0b0100011;
		funct3 = 0x2;
		rs2 = rs2C;
		rs1 = rs1C;
		instword = (Imm >> 5);
		instword = (instword << 5) + rs2;
		//	cout <<rs1 <<endl;
		instword = (instword << 5) + rs1;
		//	cout <<funct3 <<endl;
		instword = (instword << 3) + funct3;
		//	cout << bitset<8>(Imm&0x01f)<<endl;
		instword = (instword << 5) + (Imm & 0x01f);
		instword = (instword << 7) + opcode;
		//	cout << bitset<32> (instword) <<endl;
	}
	else if (funct3C == 0b010)
	{
		if (compressed_word == 0)
		{
			cout << "\tIllegal instruction" << endl;
		}
		else
		{//C.ADDI4SPN
			nzuimm = 0x00000000;
			nzuimm = (compressed_word >> 6) & 0x00000001;
			nzuimm= nzuimm| (((compressed_word >> 5) & 0x00000001)<<1);
			nzuimm = nzuimm | (((compressed_word >> 11) & 0x00000003) << 2);
			nzuimm = nzuimm | (((compressed_word >> 7) & 0x0000000f) << 4);
			nzuimm = nzuimm << 2;
			opcode = 0b0010011;
			funct3 = 0x0;

			rs2C = (compressed_word >> 2) & 0x00000007;
			rd = rs2C+8;
			rs1 = 0x00000002;
			instword = 0x00000000;
			instword = (instword << 5) | rs1;
			instword = (instword << 3) | funct3;
			instword = (instword << 5) | rd;
			instword = (instword << 7) | opcode;


		}
	}

		return instword;
}


int main(int argc, char* argv[]) {

	unsigned short instWord = 0xccc8;
	cout << hex << "0x" << (int)decompress0(instWord) << endl;

	return 0;

}