#include <iostream>	
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc = 0;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[31];
const unsigned int reg0 = 0;

//opcode with 01
unsigned int decompress1(unsigned short compressed_word) { //pc=pc+2 and is there cout //change
	unsigned int funct3_c, J_imm_c, B_imm_c1, B_imm_c2;
	unsigned int J_imm, J_imm1, J_imm2, B_imm, B_imm1, B_imm2;

	unsigned int instword = 0;
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int opC, rs2C, funct2C, rdC, funct6C;
	unsigned int instPC = pc - 4;

	opC = (compressed_word) & 0x00000003;
	rs2C = (compressed_word >> 2) & 0x00000007;
	funct2C = (compressed_word >> 5) & 0x00000003;
	rdC = (compressed_word >> 7) & 0x00000007;
	funct6C = (compressed_word >> 10) & 0x0000003F;

	funct3_c = (compressed_word >> 13) & 0x00000007;

	if (funct3_c == 0x1) { //c.jal
		J_imm_c = (compressed_word >> 2) & 0x000007FF;

		J_imm1 = (J_imm_c >> 1) & 0x00000007;//imm[3:1]
		J_imm2 = (J_imm_c >> 6) & 0x00000008;//imm[4]
		J_imm = (J_imm1 | J_imm2) & 0x0000000F;//imm[4:1]
		J_imm1 = (J_imm_c << 5) & 0x00000010; //imm[5]
		J_imm = J_imm | J_imm1; //imm[5:1]
		J_imm1 = (J_imm_c << 2) & 0x00000040; //imm[7]
		J_imm2 = J_imm1 | (J_imm_c & 0x00000020);//imm[7:6]
		J_imm = J_imm | J_imm2;
		J_imm = J_imm | (J_imm_c & 0x00000180);//imm[9:1]
		J_imm1 = (J_imm_c << 3) & 0x00000200; //imm[10]
		J_imm = J_imm | J_imm1; //imm[10:1]
		J_imm1 = (J_imm_c >> 10) & 0x00000001; //imm[11]
		J_imm = (J_imm << 1) | J_imm1; //imm[10:1|11]
		J_imm = (J_imm << 8) | ((J_imm & 0x00000001) ? 0x000000FF : 0x00000000); //imm[10:1|11|19:12]
		J_imm1 = (J_imm_c << 9) & 0x00080000;
		J_imm = J_imm | J_imm1;

		opcode = 0x0000006F;
		rd = 0x00000001;
		instword = (J_imm << 12) | (rd << 7) | opcode;
	}
	else if (funct3_c == 0x5) { //c.j
		J_imm_c = (compressed_word >> 2) & 0x000007FF;

		J_imm1 = (J_imm_c >> 1) & 0x00000007;//imm[3:1]
		J_imm2 = (J_imm_c >> 6) & 0x00000008;//imm[4]
		J_imm = (J_imm1 | J_imm2) & 0x0000000F;//imm[4:1]
		J_imm1 = (J_imm_c << 5) & 0x00000010; //imm[5]
		J_imm = J_imm | J_imm1; //imm[5:1]
		J_imm1 = (J_imm_c << 2) & 0x00000040; //imm[7]
		J_imm2 = J_imm1 | (J_imm_c & 0x00000020);//imm[7:6]
		J_imm = J_imm | J_imm2;
		J_imm = J_imm | (J_imm_c & 0x00000180);//imm[9:1]
		J_imm1 = (J_imm_c << 3) & 0x00000200; //imm[10]
		J_imm = J_imm | J_imm1; //imm[10:1]
		J_imm1 = (J_imm_c >> 10) & 0x00000001; //imm[11]
		J_imm = (J_imm << 1) | J_imm1; //imm[10:1|11]
		J_imm = (J_imm << 8) | ((J_imm & 0x00000001) ? 0x000000FF : 0x00000000); //imm[10:1|11|19:12]
		J_imm1 = (J_imm_c << 9) & 0x00080000;
		J_imm = J_imm | J_imm1;

		opcode = 0x0000006F;
		rd = 0x00000000;
		instword = (J_imm << 12) | (rd << 7) | opcode;
	}
	else if (funct3_c == 0x6) { //c.beqz
		B_imm_c1 = (compressed_word >> 2) & 0x0000001F;
		B_imm_c2 = (compressed_word >> 10) & 0x00000007;
		B_imm = (B_imm_c1 >> 1) & 0x00000003; //imm[2:1]
		B_imm = ((B_imm_c2 & 0x00000003) << 2) | B_imm; //imm[4:1]
		B_imm = ((B_imm_c1 & 0x00000001) << 5) | B_imm; //imm[5:1]
		B_imm = ((B_imm_c1 & 0x00000018) << 2) | B_imm; //imm[7:1]
		B_imm = ((B_imm_c2 & 0x00000004) << 5) | B_imm; //imm[8:1]

		B_imm1 = B_imm & 0x0000000F;//imm[4:1]
		B_imm2 = ((B_imm >> 4) & 0x0000000F);//imm[8:5]
		B_imm1 = B_imm1 << 1;
		B_imm1 = B_imm1 | ((compressed_word >> 12) & 0x00000001); //imm[4:1|11]
		B_imm2 = B_imm2 | ((B_imm_c2 & 0x00000004) ? 0x00000070 : 0x00000000); //imm[12|10:5]

		opcode = 0x00000063;
		funct3 = 0x00000000;
		rs1 = (compressed_word >> 7) & 0x00000007;
		rs2 = 0x00000000;
		instword = (B_imm2 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (B_imm1 << 7) | opcode;
	}
	else if (funct3_c == 0x7) { //c.bnez
		B_imm_c1 = (compressed_word >> 2) & 0x0000001F;
		B_imm_c2 = (compressed_word >> 10) & 0x00000007;
		B_imm = (B_imm_c1 >> 1) & 0x00000003; //imm[2:1]
		B_imm = ((B_imm_c2 & 0x00000003) << 2) | B_imm; //imm[4:1]
		B_imm = ((B_imm_c1 & 0x00000001) << 5) | B_imm; //imm[5:1]
		B_imm = ((B_imm_c1 & 0x00000018) << 2) | B_imm; //imm[7:1]
		B_imm = ((B_imm_c2 & 0x00000004) << 5) | B_imm; //imm[8:1]

		B_imm1 = B_imm & 0x0000000F;//imm[4:1]
		B_imm2 = ((B_imm >> 4) & 0x0000000F);//imm[8:5]
		B_imm1 = B_imm1 << 1;
		B_imm1 = B_imm1 | ((compressed_word >> 12) & 0x00000001); //imm[4:1|11]
		B_imm2 = B_imm2 | ((B_imm_c2 & 0x00000004) ? 0x00000070 : 0x00000000); //imm[12|10:5]

		opcode = 0x00000063;
		funct3 = 0x00000001;
		rs1 = (compressed_word >> 7) & 0x00000007;
		rs2 = 0x00000000;
		instword = (B_imm2 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (B_imm1 << 7) | opcode;
	}
	return instword;
}

unsigned int compressed(unsigned short compressed_word) {

	unsigned int opcode, instword = 0;
	opcode = compressed_word & 0x00000003;
	if (opcode == 0x1)
		instword = decompress1(compressed_word);
	return instword;
}

int main(int argc, char* argv[]) {

	unsigned short instWord = 0xE111;
	cout << hex << "0x" << (int)decompress1(instWord) << endl;

	return 0;
		
}