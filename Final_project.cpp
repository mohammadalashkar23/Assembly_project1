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
#include <cmath>

using namespace std;

unsigned int pc = 0;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[31];
const unsigned int reg0 = 0;
bool indicator = false; //for compressed jump
bool indicator2 = false;
void emitError(string s)
{
	cout << s;
	exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW) {
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}
//Ecall
void Ecall()
{
	cout << "\t" << "Ecall";
	if (reg[16] == 1) //a7
		cout << "\t" << (int)reg[9] << endl; //a0
	else if (reg[16] == 4) {
		unsigned int i = reg[9]; //i=address
		cout << "\t";
		while (memory[i] != NULL) {
			cout << memory[i];
			++i;
		}
		cout << endl;
	}
	else {
		exit(0);
	}
}
//compressed functions with opcpde '10'
unsigned int decompress2(unsigned short compressed_word)
{
	unsigned int instword;
	unsigned int rd, rs1, rs2, funct3, funct7, opcode, uimm1, uimm2, uimm;
	unsigned short opC, rs2C, rs1C, funct3C, funct4C;
	unsigned int instPC = pc - 4;
	unsigned int lw_imm;
	unsigned int shamt, nzimm_5;
	uimm1 = (compressed_word >> 2) & 0x0000001f;
	uimm2 = (compressed_word >> 12) & 0x00000001;
	funct3C = (compressed_word >> 13) & 0x00000007;
	opC = (compressed_word) & 0x00000003;
	rs2C = (compressed_word >> 2) & 0x0000001f;
	rs1C = (compressed_word >> 7) & 0x0000001f;
	funct4C = (compressed_word >> 12) & 0x0000000f;
	nzimm_5 = (compressed_word >> 12) & 0x00000001;

	if (funct4C == 0b1000) { // C.JR and C.MV

		if (rs2C == 0) {	// JR
			opcode = 0b1100111;
			funct3 = 0x0;
			funct7 = 0x00;
			rs2 = 0x00;
			rd = 0x00;
			rs1 = rs1C;
			instword = funct7;
			instword = (instword << 5) + rs2;
			instword = (instword << 5) + rs1;
			instword = (instword << 3) + funct3;
			instword = (instword << 5) + rd;
			instword = (instword << 7) + opcode;
			cout << "\tC.JR\tx" << rs1 << "\n";
		}

		else {	// MV
			opcode = 0b0110011;
			funct3 = 0x0;
			funct7 = 0x00;
			rs2 = rs2C;
			rd = rs1C;
			rs1 = 0x00;
			instword = funct7;
			instword = (instword << 5) + rs2;
			instword = (instword << 5) + rs1;
			instword = (instword << 3) + funct3;
			instword = (instword << 5) + rd;
			instword = (instword << 7) + opcode;
			cout << "\tC.MV\tx" << rd << ", x" << rs2 << "\n";
		}

	}

	else if (funct4C == 0b1001) { // C.Jalr and C.Add

		if (rs2C == 0) {	// Jalr
			opcode = 0b1100111;
			funct3 = 0x0;
			funct7 = 0x00;
			rs2 = 0x00;
			rd = 0x01;
			rs1 = rs1C;
			instword = funct7;
			instword = (instword << 5) + rs2;
			instword = (instword << 5) + rs1;
			instword = (instword << 3) + funct3;
			instword = (instword << 5) + rd;
			instword = (instword << 7) + opcode;
			cout << "\tC.JALR\tx" << rd << ", x" << rs1 << "\n";
		}

		else {	// Add
			opcode = 0b0110011;
			funct3 = 0x0;
			funct7 = 0x00;
			rs2 = rs2C;
			rd = rs1C;
			rs1 = rs1C;
			instword = funct7;
			instword = (instword << 5) + rs2;
			instword = (instword << 5) + rs1;
			instword = (instword << 3) + funct3;
			instword = (instword << 5) + rd;
			instword = (instword << 7) + opcode;
			cout << "\tC.ADD\tx" << rd << ", x" << rs2 << "\n";
		}
	}
	else if (funct3C == 6) { //swsp
		uimm1 = (compressed_word >> 7) & 0x3; //bits [7:6]
		uimm2 = (compressed_word >> 9) & 0xf; //bits [5:2]
		uimm = (uimm1 << 7) + (uimm2 << 2); //765,43200

		opcode = 0b0100011;
		funct3 = 0x2;
		rs2C = (compressed_word >> 2) & 0x1f;

		instword = (uimm >> 5);
		instword = (instword << 5) + rs2C;
		instword = (instword << 5) + 2;
		instword = (instword << 3) + funct3;
		instword = (instword << 5) + (uimm & 0x1f);
		instword = (instword << 7) + opcode;
		cout << "\tC.SWSP\tx" << rs2C << ", " << (int)uimm << "(x2)" << "\n";

	}
	else if (funct3C == 0b010) {
		lw_imm = (uimm1 >> 2) & 0x00000007;
		lw_imm = lw_imm | (uimm2 << 3);
		uimm1 = (uimm1 & 0x00000003) << 4;
		lw_imm = lw_imm | uimm1;
		lw_imm = lw_imm << 2;

		opcode = 0x00000003;
		rd = rs1C;
		funct3 = 0x2;
		rs1 = 0x2;
		instword = (lw_imm << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;

		cout << "\tC.LWSP\tx" << rd << ", " << (int)lw_imm << "(x2)" << "\n";
	}
	else if (funct3C == 0b000) { // C.slli

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
		instword = 0;


	}

	return instword;

}
//compressed functions with opcpde '01'
unsigned int decompress1(unsigned short compressed_word)
{
	unsigned int instword;
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int opC, rs1C, rs2C, funct2C, rdC, funct6C;
	unsigned int I_imm;
	unsigned int funct3C, shamt;
	unsigned int nzimm4_0;
	unsigned int nzimm_5;
	unsigned int  funct2C_2, rdC_5bits;
	unsigned int lui_imm1, lui_imm2, lui_imm3, lui_imm;
	unsigned int funct3_c, J_imm_c, B_imm_c1, B_imm_c2;
	unsigned int J_imm, J_imm1, J_imm2, B_imm, B_imm1, B_imm2;

	opC = (compressed_word) & 0x00000003;
	rs2C = (compressed_word >> 2) & 0x00000007;
	funct2C = (compressed_word >> 5) & 0x00000003;
	rdC = (compressed_word >> 7) & 0x00000007;
	funct6C = (compressed_word >> 10) & 0x0000003F;
	funct3C = (compressed_word >> 13) & 0x00000007;
	nzimm4_0 = (compressed_word >> 2) & 0x0000001f;
	rdC_5bits = (compressed_word >> 7) & 0x0000001f;
	funct2C_2 = (compressed_word >> 10) & 0x00000003;
	nzimm_5 = (compressed_word >> 12) & 0x00000001;

	if (funct6C == 0b100011) {
		switch (funct2C) {
		case 0: //sub
			opcode = 0b0110011;
			funct3 = 0x0;
			funct7 = 0x20;
			rs2 = rs2C + 8;
			rd = rdC + 8;
			rs1 = rd;
			instword = funct7;
			instword = (instword << 5) + rs2;
			instword = (instword << 5) + rs1;
			instword = (instword << 3) + funct3;
			instword = (instword << 5) + rd;
			instword = (instword << 7) + opcode;
			cout << "\tC.SUB\tx" << rd << ", x" << rs2 << "\n";
			break;

		case 1: // Xor
			opcode = 0b0110011;
			funct3 = 0x4;
			funct7 = 0x00;
			rs2 = rs2C + 8;
			rd = rdC + 8;
			rs1 = rd;
			instword = funct7;
			instword = (instword << 5) + rs2;
			instword = (instword << 5) + rs1;
			instword = (instword << 3) + funct3;
			instword = (instword << 5) + rd;
			instword = (instword << 7) + opcode;
			cout << "\tC.XOR\tx" << rd << ", x" << rs2 << "\n";
			break;

		case 2:	//Or
			opcode = 0b0110011;
			funct3 = 0x6;
			funct7 = 0x00;
			rs2 = rs2C + 8;
			rd = rdC + 8;
			rs1 = rd;
			instword = funct7;
			instword = (instword << 5) + rs2;
			instword = (instword << 5) + rs1;
			instword = (instword << 3) + funct3;
			instword = (instword << 5) + rd;
			instword = (instword << 7) + opcode;
			cout << "\tC.OR\tx" << rd << ", x" << rs2 << "\n";
			break;

		case 3:	//And
			opcode = 0b0110011;
			funct3 = 0x7;
			funct7 = 0x00;
			rs2 = rs2C + 8;
			rd = rdC + 8;
			rs1 = rd;
			instword = funct7;
			instword = (instword << 5) + rs2;
			instword = (instword << 5) + rs1;
			instword = (instword << 3) + funct3;
			instword = (instword << 5) + rd;
			instword = (instword << 7) + opcode;
			cout << "\tC.AND\tx" << rd << ", x" << rs2 << "\n";
			break;
		default:
			cout << "\tUnkown Compressed Instruction \n";
			//intialize the fields with zeros to ensures that it is not mapped to an instruction in RV32I


			instword = 0;
		}
	}
	else if (funct3C == 0b000)
	{
		if (compressed_word == 0x0001)//NOP
		{
			instword = 0x00000013;
		}
		else //c.addi
		{
			opcode = 0b0010011;
			funct3 = 0x0;

			rd = rdC_5bits;
			rs1 = rdC_5bits;
			I_imm = (nzimm4_0) | ((nzimm_5) ? 0x00000fe0 : 0x00000000);//make sure
			instword = I_imm;
			instword = (instword << 5) | rs1;
			instword = (instword << 3) | funct3;
			instword = (instword << 5) | rd;
			instword = (instword << 7) | opcode;
			cout << "\tC.ADDI\tx" << rd << ", " << hex << "0x" << (int)I_imm << "\n";
		}
	}
	else if (funct3C == 0b100)
	{
		if (funct2C_2 == 0b00)
		{
			if (nzimm4_0 == 0b00000)
			{
				cout << "\tunknown instruction" << endl;
			}
			else//c.srli
			{
				opcode = 0b0010011;
				funct3 = 0x5;

				rd = rdC + 8;
				rs1 = rdC + 8;
				shamt = nzimm4_0;
				instword = 0x00000000;
				instword = (instword << 5) | shamt;
				instword = (instword << 5) | rs1;
				instword = (instword << 3) | funct3;
				instword = (instword << 5) | rd;
				instword = (instword << 7) | opcode;
				cout << "\tC.SRLI\tx" << rd << ", " << hex << "0x" << (int)shamt << "\n";
			}
		}
		else if (funct2C_2 == 0b01)
		{
			if (nzimm4_0 == 0b00000)
			{
				cout << "\tunknown instruction" << endl;
			}
			else//c.srli
			{
				opcode = 0b0010011;
				funct3 = 0x5;

				rd = rdC + 8;
				rs1 = rdC + 8;
				shamt = nzimm4_0;
				instword = 0x00000020;
				instword = (instword << 5) | shamt;
				instword = (instword << 5) | rs1;
				instword = (instword << 3) | funct3;
				instword = (instword << 5) | rd;
				instword = (instword << 7) | opcode;
				cout << "\tC.SRAI\tx" << rd << ", " << hex << "0x" << (int)shamt << "\n";
			}
		}
		else if (funct2C_2 == 0b10) { //ANDI

			opcode = 0x0010011;
			funct3 = 0x7;
			lui_imm1 = (compressed_word >> 12) & 0x1; //bit number [5]
			lui_imm2 = (compressed_word >> 2) & 0x1f; //bit [4:0]
			rs1C = (compressed_word >> 7) & 0x7;
			rdC = rs1C + 8;
			instword = (lui_imm1); //bit[5]
			instword = (instword << 5) + lui_imm2; //bit[4:0]
			instword = (instword << 5) + rs1C;
			instword = (instword << 3) + funct3;
			instword = (instword << 5) + rdC;
			instword = (instword << 7) + opcode;

			lui_imm1 = ((lui_imm1) ? 0xffffffff : 0x0);
			lui_imm1 = (lui_imm1 << 5) | lui_imm2;
			cout << "\tC.ANDI\tx" << rdC << ", " << hex << "0x" << (int)lui_imm1 << "\n";

		}
	}
	else if (funct3C == 0b011)
	{

		if (rdC_5bits == 2)
		{//c.addi16sp
			unsigned int add16_imm;
			add16_imm = (nzimm4_0 >> 4) & 0x00000001;
			add16_imm = add16_imm | ((nzimm4_0 & 0x00000001) << 1);
			add16_imm = add16_imm | (((nzimm4_0 >> 3) & 0x00000001) << 2);
			add16_imm = add16_imm | (((nzimm4_0 >> 1) & 0x00000001) << 3);
			add16_imm = add16_imm | (((nzimm4_0 >> 2) & 0x00000001) << 4);
			add16_imm = add16_imm | ((nzimm_5) << 5);
			I_imm = (add16_imm) | ((nzimm_5) ? 0xffffffc0 : 0x00000000);
			I_imm = I_imm << 4;
			opcode = 0b0010011;
			funct3 = 0x0;
			rd = rdC_5bits;
			rs1 = rdC_5bits;
			instword = I_imm;
			instword = (instword << 5) | rs1;
			instword = (instword << 3) | funct3;
			instword = (instword << 5) | rd;
			instword = (instword << 7) | opcode;

			cout << "\tC.ADDI16SP\tx" << rd << ", " << hex << "0x" << (int)I_imm << "\n";
		}
		else //lui
		{
			funct3C = (compressed_word >> 13);
			lui_imm1 = (compressed_word >> 12) & 0x1; //bit [17]
			lui_imm2 = (compressed_word >> 2) & 0x1f; //bit[16:12]
			rdC = (compressed_word >> 7) & 0x1f; // c.lui 
			opcode = 0b0110111;
			lui_imm1 = ((lui_imm1) ? 0xffffffff : 0x00000000);
			lui_imm1 = (lui_imm1 << 5) | lui_imm2;
			instword = (lui_imm1);

			instword = (instword << 5) + rdC;
			instword = (instword << 7) + opcode;
			lui_imm1 = (lui_imm1 << 1);
			cout << "\tC.LUI\tx" << rdC << ", " << hex << "0x" << ((int)lui_imm1 >> 3) << "\n";

		}
	}
	else if (funct3C == 0b010) { //li 
		funct3C = (compressed_word >> 13);
		lui_imm1 = (compressed_word >> 12) & 0x1; //bit [5]
		lui_imm2 = (compressed_word >> 2) & 0x1f; //bit[4:0]
		rdC = (compressed_word >> 7) & 0x1f;
		if (funct3C == 2) {//c.li to addi 
			opcode = 0b0010011;
			funct3 = 0x0;
			lui_imm1 = (lui_imm1) ? 0xffffffff : 0x00000000;
			instword = (lui_imm1); //bit[5] extended...
			instword = (instword << 5) + lui_imm2; //bit[4:0]
			instword = (instword << 5) + 0;
			instword = (instword << 3) + funct3;
			instword = (instword << 5) + rdC;
			instword = (instword << 7) + opcode;

			lui_imm1 = (lui_imm1 << 5) + lui_imm2; //the extended immediate 

			cout << "\tC.LI\tx" << rdC << ", " << hex << "0x" << (int)lui_imm1 << "\n";
		}
		else
		{
			cout << "\tUnkown Compressed Instruction \n";

		}
	}
	else if (funct3C == 0x1) { //c.jal
		J_imm_c = (compressed_word >> 2) & 0x000007FF;

		J_imm1 = (J_imm_c >> 1) & 0x00000007;//imm[3:1]
		J_imm2 = (J_imm_c >> 9) & 0x00000001;//imm[4]
		J_imm = (J_imm1 | (J_imm2 << 3)) & 0x0000000F;//imm[4:1]
		J_imm1 = (J_imm_c & 0x00000001); //imm[5]
		J_imm = J_imm | (J_imm1 << 4); //imm[5:1]
		J_imm1 = (J_imm_c >> 5) & 0x00000001; //imm[6]
		J_imm = J_imm | (J_imm1 << 5); //imm[6:1]
		J_imm1 = (J_imm_c >> 4) & 0x00000001; //imm[7]
		J_imm = J_imm | (J_imm1 << 6); //imm[7:1]
		J_imm1 = (J_imm_c >> 7) & 0x00000003; //imm[9:8]
		J_imm = J_imm | (J_imm1 << 7); //imm[9:1]
		J_imm1 = (J_imm_c >> 6) & 0x00000001; //imm[9:8]
		J_imm = J_imm | (J_imm1 << 9); //imm[10:1]
		J_imm1 = (J_imm_c >> 10) & 0x00000001; //imm[11]
		J_imm = J_imm | (J_imm1 << 10); //imm[11:1]
		unsigned temp_J_imm = J_imm;
		temp_J_imm = temp_J_imm | ((J_imm1) ? 0xFFFFF800 : 0x00000000);
		temp_J_imm = temp_J_imm << 1;

		J_imm = (J_imm << 1) | J_imm1;//imm[20|10:1|11]
		J_imm = (J_imm << 8) | ((J_imm1) ? 0x000000FF : 0x00000000); //imm[10:1|11|19:12]

		opcode = 0x0000006F;
		rd = 0x00000001;
		instword = (J_imm << 12) | (rd << 7) | opcode;
		indicator = true;

		cout << "\tC.JAL\tx1  " << hex << "0x" << (int)temp_J_imm + (pc - 2) << "\n";
	}
	else if (funct3C == 0x5) { //c.j
		J_imm_c = (compressed_word >> 2) & 0x000007FF;

		J_imm1 = (J_imm_c >> 1) & 0x00000007;//imm[3:1]
		J_imm2 = (J_imm_c >> 9) & 0x00000001;//imm[4]
		J_imm = (J_imm1 | (J_imm2 << 3)) & 0x0000000F;//imm[4:1]
		J_imm1 = (J_imm_c & 0x00000001); //imm[5]
		J_imm = J_imm | (J_imm1 << 4); //imm[5:1]
		J_imm1 = (J_imm_c >> 5) & 0x00000001; //imm[6]
		J_imm = J_imm | (J_imm1 << 5); //imm[6:1]
		J_imm1 = (J_imm_c >> 4) & 0x00000001; //imm[6]
		J_imm = J_imm | (J_imm1 << 6); //imm[7:1]
		J_imm1 = (J_imm_c >> 7) & 0x00000003; //imm[9:8]
		J_imm = J_imm | (J_imm1 << 7); //imm[9:1]
		J_imm1 = (J_imm_c >> 6) & 0x00000001; //imm[9:8]
		J_imm = J_imm | (J_imm1 << 9); //imm[10:1]
		J_imm1 = (J_imm_c >> 10) & 0x00000001; //imm[11]
		J_imm = J_imm | (J_imm1 << 10); //imm[11:1]
		unsigned temp_J_imm = J_imm;
		temp_J_imm = temp_J_imm | ((J_imm1) ? 0xFFFFF800 : 0x00000000);
		temp_J_imm = temp_J_imm << 1;

		J_imm = (J_imm << 1) | J_imm1;//imm[20|10:1|11]
		J_imm = (J_imm << 8) | ((J_imm1) ? 0x000000FF : 0x00000000); //imm[10:1|11|19:12]

		opcode = 0x0000006F;
		rd = 0x00000000;
		instword = (J_imm << 12) | (rd << 7) | opcode;
		cout << "\tC.J\t" << hex << "0x" << (int)temp_J_imm + (pc - 2) << "\n";
		indicator = true;
	}
	else if (funct3C == 0x6) { //c.beqz
		indicator = true;
		B_imm_c1 = (compressed_word >> 2) & 0x0000001F;
		B_imm_c2 = (compressed_word >> 10) & 0x00000007;
		B_imm = (B_imm_c1 >> 1) & 0x00000003; //imm[2:1]
		B_imm = ((B_imm_c2 & 0x00000003) << 2) | B_imm; //imm[4:1]
		B_imm = ((B_imm_c1 & 0x00000001) << 4) | B_imm; //imm[5:1]
		B_imm = ((B_imm_c1 & 0x00000018) << 2) | B_imm; //imm[7:1]
		B_imm = ((B_imm_c2 & 0x00000004) << 5) | B_imm; //imm[8:1]

		unsigned int temp_imm = B_imm;
		temp_imm = temp_imm | (((B_imm >> 7) & 0x1) ? 0xFFFFFF00 : 0x00000000);
		temp_imm = temp_imm << 1;

		B_imm1 = B_imm & 0x0000000F;//imm[4:1]
		B_imm2 = ((B_imm >> 4) & 0x0000000F);//imm[8:5]
		B_imm1 = B_imm1 << 1;
		B_imm1 = B_imm1 | ((compressed_word >> 12) & 0x00000001); //imm[4:1|11]
		B_imm2 = B_imm2 | ((B_imm_c2 & 0x00000004) ? 0x00000070 : 0x00000000); //imm[12|10:5]


		opcode = 0x00000063;
		funct3 = 0x00000000;
		rs1 = ((compressed_word >> 7) & 0x00000007) + 8;
		rs2 = 0x00000000;
		instword = (B_imm2 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (B_imm1 << 7) | opcode;
		cout << "\tC.BEQZ\tx" << rs1 << ", " << hex << "0x" << temp_imm+(pc-2) << "\n";
	}
	else if (funct3C == 0x7) { //c.bnez
		indicator = true;
		B_imm_c1 = (compressed_word >> 2) & 0x0000001F;
		B_imm_c2 = (compressed_word >> 10) & 0x00000007;
		B_imm = (B_imm_c1 >> 1) & 0x00000003; //imm[2:1]
		B_imm = ((B_imm_c2 & 0x00000003) << 2) | B_imm; //imm[4:1]
		B_imm = ((B_imm_c1 & 0x00000001) << 4) | B_imm; //imm[5:1]
		B_imm = ((B_imm_c1 & 0x00000018) << 2) | B_imm; //imm[7:1]
		B_imm = ((B_imm_c2 & 0x00000004) << 5) | B_imm; //imm[8:1]

		unsigned int temp_imm = B_imm;
		temp_imm = temp_imm | (((B_imm >> 7) & 0x1) ? 0xFFFFFF00 : 0x00000000);
		temp_imm = temp_imm << 1;

		B_imm1 = B_imm & 0x0000000F;//imm[4:1]
		B_imm2 = ((B_imm >> 4) & 0x0000000F);//imm[8:5]
		B_imm1 = B_imm1 << 1;
		B_imm1 = B_imm1 | ((compressed_word >> 12) & 0x00000001); //imm[4:1|11]
		B_imm2 = B_imm2 | ((B_imm_c2 & 0x00000004) ? 0x00000070 : 0x00000000); //imm[12|10:5]

		opcode = 0x00000063;
		funct3 = 0x00000001;
		rs1 = ((compressed_word >> 7) & 0x00000007) + 8;
		rs2 = 0x00000000;
		instword = (B_imm2 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (B_imm1 << 7) | opcode;
		cout << "\tC.BNEZ\tx" << rs1 << ", " << hex << "0x" << temp_imm + (pc-2) << "\n";
	}
	else
	{
		cout << "\tUnkown Compressed Instruction \n";

	}
	//insert the if else for different instruction with opcode =01, but with different func6C or funct3C here
	return instword;

}

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
		rdC = (compressed_word >> 2) & 0x00000007;
		Imm1 = (compressed_word >> 6) & 0x1;   //the second bit in the Imm1
		Imm2 = (compressed_word >> 10) & 0x7;  //get the [3:5] bits
		Imm3 = (compressed_word >> 5) & 0x1; //get the 6 bit
		Imm = (Imm1 << 2) | (Imm2 << 3) | (Imm3 << 6);
		rs1C = (compressed_word >> 7) & 0x00000007;

		opcode = 0b0000011;
		funct3 = 0x2;
		rd = rdC + 8;
		rs1 = rs1C + 8;
		instword = Imm;
		instword = (instword << 5) + rs1;
		instword = (instword << 3) + funct3;
		instword = (instword << 5) + rd;
		instword = (instword << 7) + opcode;
		cout << "\tC.LW\tx" << rd << ", " << hex << (int)Imm << " (x" << rs1 << ')' << "\n";
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
		cout << "\tC.SW\tx" << rs2 << ", " << hex << (int)Imm << " (x" << rs1 << ')' << "\n";
	}
	else if (funct3C == 0b000)
	{
		if (compressed_word == 0)
		{
			cout << "\tIllegal instruction" << endl;
			instword = 0x00000000;
		}
		else
		{//C.ADDI4SPN
			nzuimm = 0x00000000;
			nzuimm = (compressed_word >> 6) & 0x00000001;
			nzuimm = nzuimm | (((compressed_word >> 5) & 0x00000001) << 1);
			nzuimm = nzuimm | (((compressed_word >> 11) & 0x00000003) << 2);
			nzuimm = nzuimm | (((compressed_word >> 7) & 0x0000000f) << 4);
			nzuimm = nzuimm << 2;
			opcode = 0b0010011;
			funct3 = 0x0;

			rs2C = (compressed_word >> 2) & 0x00000007;
			rd = rs2C + 8;
			rs1 = 0x00000002;
			instword = nzuimm;
			instword = (instword << 5) | rs1;
			instword = (instword << 3) | funct3;
			instword = (instword << 5) | rd;
			instword = (instword << 7) | opcode;
			cout << "\tC.ADDI4SPN\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)nzuimm << "\n";

		}

	}
	else {
		instword = 0;

	}


	return instword;
}

unsigned int decompress(unsigned short compressed_word) {
	indicator2 = true;
	unsigned int opcode, instword = 0;
	opcode = compressed_word & 0x00000003;
	if (opcode == 0x0)
		instword = decompress0(compressed_word);
	else if (opcode == 0x1)
		instword = decompress1(compressed_word);
	else if (opcode == 0x2)
		instword = decompress2(compressed_word);
	else
		cout << "unkown compressed instruction" << endl;
	return instword;
}
//I type
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

	// � inst[31] � inst[30:25] inst[24:21] inst[20]
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

	unsigned shamt;
	shamt = I_imm & 0x01F;
	funct7 = (I_imm >> 5) & 0x07F;

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
				reg[rd - 1] = reg[rs1 - 1] + I_imm;
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
				reg[rd - 1] = reg[rs1 - 1] * (pow(2, shamt));
			}
			break;
		case 2:	cout << "\tSLTI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				if (signed(reg0) < signed(I_imm))
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

				if (signed(reg[rs1 - 1]) < signed(I_imm))
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
	else if (opcode == 0x3) {
		switch (funct3) {
		case 0:	cout << "\tLB\tx" << rd << ", " << (int)I_imm << " (x" << rs1 << ')' << "\n";
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
		case 1:	cout << "\tLH\tx" << rd << ", " << (int)I_imm << " (x" << rs1 << ')' << "\n";
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
		case 2:	cout << "\tLW\tx" << rd << ", " << (int)I_imm << " (x" << rs1 << ')' << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				int temp = I_imm;
				if ((temp >= 0) && (temp + 3 <= sizeof(memory) - 1))
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
			else
			{
				int temp = I_imm + reg[rs1 - 1];
				if ((temp >= 0) && (temp + 3 <= sizeof(memory) - 1))
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
			break;
		case 5:	cout << "\tLHU\tx" << rd << ", " << (int)I_imm << " (x" << rs1 << ')' << "\n";
			if (rd == 0)
			{

			}
			else if (rs1 == 0)
			{
				int temp = I_imm;
				if ((temp >= 0) && (temp + 1 <= sizeof(memory) - 1))
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
				if ((temp >= 0) && (temp + 1 <= sizeof(memory) - 1))
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
	else {
		cout << "\tJALR\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
		if (rd != 0)
			reg[rd - 1] = pc;
		if (rs1 != 0)
			pc = reg[rs1 - 1] + I_imm;
		else
			pc = reg0 + I_imm;
	}
}
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

	switch (funct3) {
	case 0:
		if (funct7 == 32) {
			cout << "\tSUB\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			if (rd == 0) {}
			else if (rs1 == 0) {
				if (rs2 == 0) {
					reg[rd - 1] = 0;
				}
				reg[rd - 1] = 0 - reg[rs2 - 1];
			}
			else if (rs2 == 0)
				reg[rd - 1] = reg[rs1 - 1] - 0;
			else
				reg[rd - 1] = reg[rs1 - 1] - reg[rs2 - 1];
		}
		else if (funct7 == 0) {
			cout << "\tADD\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			if (rd == 0) {}
			else if (rs1 == 0) {
				if (rs2 == 0) {
					reg[rd - 1] = 0;
				}
				reg[rd - 1] = reg[rs2 - 1];
			}
			else if (rs2 == 0)
				reg[rd - 1] = reg[rs1 - 1];
			else
				reg[rd - 1] = reg[rs1 - 1] + reg[rs2 - 1];
		}
		else {
			cout << "\tUnkown B Instruction \n";
		}
		break;

	case 1:
	{
		cout << "\tSLL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		if (rd == 0) {}
		else if (rs1 == 0) {
			reg[rd - 1] = 0;
		}
		else if (rs2 == 0)
			reg[rd - 1] = reg[rs1 - 1];
		else
			reg[rd - 1] = reg[rs1 - 1] << reg[rs2 - 1];
	}
	break;

	case 2:
	{
		cout << "\tSLT\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		if (rd == 0) {}
		else if (rs1 == 0) {
			if (rs2 == 0) {
				reg[rd - 1] = 0;
			}
			reg[rd - 1] = (0 < static_cast<int>(reg[rs2 - 1])) ? 1 : 0;
		}
		else if (rs2 == 0)
			reg[rd - 1] = (static_cast<int>(reg[rs1 - 1]) < 0) ? 1 : 0;
		else
			reg[rd - 1] = (static_cast<int>(reg[rs1 - 1]) < static_cast<int>(reg[rs2 - 1])) ? 1 : 0;
	}
	break;

	case 3:
	{
		cout << "\tSLTU\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		if (rd == 0) {}
		else if (rs1 == 0) {
			if (rs2 == 0) {
				reg[rd - 1] = 0;
			}
			reg[rd - 1] = (0 < reg[rs2 - 1]) ? 1 : 0;
		}
		else if (rs2 == 0)
			reg[rd - 1] = (reg[rs1 - 1] < 0) ? 1 : 0;
		else
			reg[rd - 1] = (reg[rs1 - 1] < reg[rs2 - 1]) ? 1 : 0;
	}
	break;

	case 4:
	{
		cout << "\tXOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		if (rd == 0) {}
		else if (rs1 == 0) {
			if (rs2 == 0) {
				reg[rd - 1] = 0 ^ 0;
			}
			reg[rd - 1] = 0 ^ reg[rs2 - 1];
		}
		else if (rs2 == 0)
			reg[rd - 1] = reg[rs1 - 1] ^ 0;
		else
			reg[rd - 1] = reg[rs1 - 1] ^ reg[rs2 - 1];
	}
	break;

	case 5:
		if (funct7 == 32) {
			cout << "\tSRA\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			if (rd == 0) {}
			else if (rs1 == 0) {
				reg[rd - 1] = 0;
			}
			else if (rs2 == 0)
				reg[rd - 1] = reg[rs1 - 1];
			else
				reg[rd - 1] = static_cast<int>(reg[rs1 - 1]) >> reg[rs2 - 1];
		}
		else if (funct7 == 0) {
			cout << "\tSRL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			if (rd == 0) {}
			else if (rs1 == 0) {
				reg[rd - 1] = 0;
			}
			else if (rs2 == 0)
				reg[rd - 1] = reg[rs1 - 1];
			else
				reg[rd - 1] = reg[rs1 - 1] >> reg[rs2 - 1];
		}
		else {
			cout << "\tUnkown B Instruction \n";
		}
		break;

	case 6:
	{
		cout << "\tOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		if (rd == 0) {}
		else if (rs1 == 0) {
			if (rs2 == 0) {
				reg[rd - 1] = 0;
			}
			reg[rd - 1] = 0 | reg[rs2 - 1];
		}
		else if (rs2 == 0)
			reg[rd - 1] = reg[rs1 - 1] | 0;
		else
			reg[rd - 1] = reg[rs1 - 1] | reg[rs2 - 1];
	}
	break;

	case 7:
	{
		cout << "\tAND\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		if (rd == 0) {}
		else if (rs1 == 0) {
			if (rs2 == 0) {
				reg[rd - 1] = 0;
			}
			reg[rd - 1] = 0 & reg[rs2 - 1];
		}
		else if (rs2 == 0)
			reg[rd - 1] = reg[rs1 - 1] & 0;
		else
			reg[rd - 1] = reg[rs1 - 1] & reg[rs2 - 1];
	}
	break;

	default:
		cout << "\tUnkown B Instruction \n";

	}

}

//s type
void S_Type(unsigned int instWord) {
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;

	unsigned int instPC;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;

	unsigned int temp = ((instWord >> 25) & 0x3F);
	S_imm = ((instWord >> 7) & 0x1F) | (temp << 5) | (((instWord >> 31) & 0x1) ? 0xFFFFF800 : 0x0);
	//  reg[rs1-1] = 10004; //for testing
	//  reg[rs2-1] = 0b01010101010101010101010101010101;  //for testing
	switch (funct3) {
	case 0: {
		if (rs1 == 0 && rs2 == 0) {
			instPC = reg0 + S_imm;
			if (signed(instPC) >= 0) {
				memory[instPC] = 0;
				cout << "\tSB\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
			}
			else
				cout << "\texecution terminated with errors.\n";
		}
		else if (rs1 != 0 && rs2 == 0) {
			instPC = reg[rs1 - 1] + S_imm;
			if (signed(instPC) >= 0) {
				memory[instPC] = 0;
				cout << "\tSB\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
			}
			else
				cout << "\texecution terminated with errors.\n";
		}
		else if (rs2 != 0 && rs1 == 0) {
			instPC = reg0 + S_imm;
			if (signed(instPC) >= 0) {
				memory[instPC] = reg[rs2 - 1] & 0xFF;
				cout << "\tSB\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
			}
			else
				cout << "\texecution terminated with errors.\n";
		}
		else {
			instPC = reg[rs1 - 1] + S_imm;
			if (instPC < sizeof(memory) && signed(instPC) >= 0) {
				memory[instPC] = reg[rs2 - 1] & 0xFF;
				cout << "\tSB\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
			}
			else
				return;
		}
	}
		  break;

	case 1: {
		if (rs2 == 0 && rs1 == 0) {
			instPC = reg0 + S_imm;
			if (signed(instPC) >= 0) {
				memory[instPC] = 0;
				memory[instPC + 1] = 0;
				cout << "\tSH\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

			}
			else
				cout << "\texecution terminated with errors.\n";
		}
		else if (rs2 == 0 && rs1 != 0) {
			instPC = reg[rs1 - 1] + S_imm;
			if (signed(instPC) >= 0) {
				memory[instPC] = 0;
				memory[instPC + 1] = 0;
				cout << "\tSH\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

			}
			else
				cout << "\texecution terminated with errors.\n";
		}
		else if (rs2 != 0 && rs1 == 0) {
			instPC = reg0 + S_imm;
			if (signed(instPC) >= 0) {
				memory[instPC] = reg[rs2 - 1] & 0xFF;
				memory[instPC + 1] = (reg[rs2 - 1] >> 8) & 0xFF;
				cout << "\tSH\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

			}
			else
				cout << "\texecution terminated with errors.\n";
		}
		else {
			instPC = reg[rs1 - 1] + S_imm;
			if (instPC < sizeof(memory) - 1 && signed(instPC) >= 0) {
				memory[instPC] = reg[rs2 - 1] & 0xFF;
				memory[instPC + 1] = (reg[rs2 - 1] >> 8) & 0xFF;
				cout << "\tSH\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

			}
			else
				cout << "\texecution terminated with errors.\n";
		}
		break;
	}
	case 2: {
		if (rs2 == 0 && rs1 == 0) {
			instPC = reg0 + S_imm;
			if (signed(instPC) >= 0) {
				memory[instPC] = 0;
				memory[instPC + 1] = 0;
				memory[instPC + 2] = 0;
				memory[instPC + 3] = 0;
				cout << "\tSW\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

			}
			else
				cout << "\texecution terminated with errors.\n";
		}
		else if (rs2 == 0 && rs1 != 0) {
			instPC = reg[rs1 - 1] + S_imm;
			if (instPC < sizeof(memory) - 3 && signed(instPC) >= 0) {
				memory[instPC] = 0;
				memory[instPC + 1] = 0;
				memory[instPC + 2] = 0;
				memory[instPC + 3] = 0;
				cout << "\tSW\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

			}
			else
				cout << "\texecution terminated with errors.\n";
		}
		else if (rs2 != 0 && rs1 == 0) {
			instPC = reg0 + S_imm;
			if (signed(instPC) >= 0) {
				memory[instPC] = reg[rs2 - 1] & 0xFF;
				memory[instPC + 1] = (reg[rs2 - 1] >> 8) & 0xFF;
				memory[instPC + 2] = (reg[rs2 - 1] >> 16) & 0xFF;
				memory[instPC + 3] = (reg[rs2 - 1] >> 24) & 0xFF;
				cout << "\tSW\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

			}
			else
				cout << "\texecution terminated with errors.\n";
		}
		else {
			instPC = reg[rs1 - 1] + S_imm;
			if (instPC < sizeof(memory) - 3 && signed(instPC) >= 0) {
				memory[instPC] = reg[rs2 - 1] & 0xFF;
				memory[instPC + 1] = (reg[rs2 - 1] >> 8) & 0xFF;
				memory[instPC + 2] = (reg[rs2 - 1] >> 16) & 0xFF;
				memory[instPC + 3] = (reg[rs2 - 1] >> 24) & 0xFF;
				cout << "\tSW\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

			}
			else
				cout << "\texecution terminated with errors.\n";
		}
		break;
	}
	}
}

//u type 
void U_Type(unsigned int instWord) {
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;


	U_imm = ((instWord >> 12) & 0x7FFFF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
	if (rd != 0) {
		switch (opcode) {
		case 55: {
			cout << "\tLUI\tx" << rd << ", 0x" << std::hex << signed(U_imm) << "\n";
			reg[rd - 1] = U_imm << 12;
			break;
		}
		case 23: {
			cout << "\tAUIPC\tx" << rd << ", 0x" << std::hex << signed(U_imm) << "\n";
			reg[rd - 1] = U_imm << 12 + (pc - 4);
			break;
		}
		}
	}
	else {
		if (opcode == 55)
			cout << "\tLUI\tx" << reg0 << ", 0x" << std::hex << signed(U_imm) << "\n";
		else
			cout << "\tAUIPC\tx" << reg0 << ", 0x" << std::hex << signed(U_imm) << "\n";
	}
}

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
	if (indicator == true) {
		target_address = (pc - 2) + B_imm;
		indicator = false;
	}
	else
		target_address = (pc - 4) + B_imm;
	switch (funct3) {
	case 0: { cout << "\tBEQ\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)target_address << "\n";
		if (rs1 == 0 && rs2 == 0)
			pc = target_address;
		else if (rs1 == 0 && rs2 != 0) {
			if (reg0 == reg[rs2 - 1])
				pc = target_address;
		}
		else if (rs1 != 0 && rs2 == 0) {
			if (reg[rs1 - 1] == reg0)
				pc = target_address;
		}
		else {
			if (reg[rs1 - 1] == reg[rs2 - 1])
				pc = target_address;
		}
	}
		  break;
	case 1: {cout << "\tBNE\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)target_address << "\n";
		if (rs1 != 0 && rs2 != 0)
		{
			if (reg[rs1 - 1] != reg[rs2 - 1])
				pc = target_address;
		}
		else if (rs1 == 0 && rs2 != 0) {
			if (reg0 != reg[rs2 - 1])
				pc = target_address;
		}
		else if (rs1 != 0 && rs2 == 0) {
			if (reg[rs1 - 1] != reg0)
				pc = target_address;
		}
	}
		  break;
	case 4: {cout << "\tBLT\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)target_address << "\n";
		if (rs1 != 0 && rs2 != 0)
		{
			if ((int)reg[rs1 - 1] < (int)reg[rs2 - 1])
				pc = target_address;
		}
		else if (rs1 == 0 && rs2 != 0) {
			if (reg0 < (int)reg[rs2 - 1])
				pc = target_address;
		}
		else if (rs1 != 0 && rs2 == 0) {
			if ((int)reg[rs1 - 1] < (int)reg0)
				pc = target_address;
		}
	}
		  break;
	case 5: {cout << "\tBGE\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)target_address << "\n";
		if (rs1 == 0 && rs2 == 0)
			pc = target_address;
		else if (rs1 == 0 && rs2 != 0) {
			if ((int)reg0 >= (int)reg[rs2 - 1])
				pc = target_address;
		}
		else if (rs1 != 0 && rs2 == 0) {
			if ((int)reg[rs1 - 1] >= (int)reg0)
				pc = target_address;
		}
		else {
			if ((int)reg[rs1 - 1] >= (int)reg[rs2 - 1])
				pc = target_address;
		}
	}
		  break;
	case 6: {cout << "\tBLTU\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)target_address << "\n";
		if (rs1 != 0 && rs2 != 0)
		{
			if (reg[rs1 - 1] < reg[rs2 - 1])
				pc = target_address;
		}
		else if (rs1 == 0 && rs2 != 0) {
			if (reg0 < reg[rs2 - 1])
				pc = target_address;
		}
		else if (rs1 != 0 && rs2 == 0) {
			if (reg[rs1 - 1] < reg0)
				pc = target_address;
		}
	}
		  break;
	case 7: {cout << "\tBGEU\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)target_address << "\n";
		if (rs1 == 0 && rs2 == 0)
			pc = target_address;
		else if (rs1 == 0 && rs2 != 0) {
			if (reg0 >= reg[rs2 - 1])
				pc = target_address;
		}
		else if (rs1 != 0 && rs2 == 0) {
			if (reg[rs1 - 1] >= reg0)
				pc = target_address;
		}
		else {
			if (reg[rs1 - 1] >= reg[rs2 - 1])
				pc = target_address;
		}
	}
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
	if (rd != 0)
		reg[rd - 1] = pc;
	pc = (pc - 4) + J_imm;
	if (indicator) {
		pc += 2;
		indicator = false;
	}
	cout << "\tJAL\tx" << rd << ", " << hex << "0x" << ((int)pc) << "\n";
}

void instDecExec(unsigned int instWord)
{
	unsigned int rd, rs1, rs2, funct3, funct7 = 0, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;

	unsigned int instPC = pc - 4;
	if (indicator2)
	{
		instPC += 2;
		indicator2 = false;

	}

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;

	// � inst[31] � inst[30:25] inst[24:21] inst[20]
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

	printPrefix(instPC, instWord);

	if (opcode == 0x33) {		// R Instructions
		R_Type(instWord);
	}
	else if (opcode == 0x13 || opcode == 0x3 || opcode == 0x67) {	// I instructions
		I_type(instWord);
	}
	else if (opcode == 0x63) { // B instructions
		B_Type(instWord);
	}
	else if (opcode == 0x6F) { //J instructions
		J_Type(instWord);
	}
	else if (opcode == 0x37 || opcode == 0x17) {
		U_Type(instWord);
	}
	else if (opcode == 0x23) {
		S_Type(instWord);
	}
	else if (opcode == 0x73) {
		Ecall();
	}
	else {
		cout << "\tUnkown Instruction \n";

	}

}

int main(int argc, char* argv[]) {

	unsigned int instWord = 0, instWord_comp = 0;
	ifstream inFile;
	ofstream outFile;

	//read data section
	ifstream inFile_data;
	string dataname;
	cout << "enter the data section file name: " << endl;
	cin >> dataname;
	inFile_data.open(dataname, ios::in | ios::binary | ios::ate);
	if (inFile_data.is_open()) {
		int fsize1 = inFile_data.tellg();
		inFile_data.seekg(0, inFile_data.beg);
		if (!inFile_data.read((char*)(memory + 0x00010000), fsize1)) emitError("Cannot read from input file\n");
	}
	else {
		cout << "Not opened data file" << endl;
	}


	if (argc < 1) emitError("use: rvcdiss <machine_code_file_name>\n");

	inFile.open(argv[1], ios::in | ios::binary | ios::ate);

	if (inFile.is_open())
	{
		int fsize = inFile.tellg();
		inFile.seekg(0, inFile.beg);
		if (!inFile.read((char*)memory, fsize)) emitError("Cannot read from input file\n");

		while (true) { // uncompressed instruction
			if ((memory[pc] & 0x3) == 3) {
				instWord = memory[pc] |
					((memory[pc + 1]) << 8) |
					((memory[pc + 2]) << 16) |
					((memory[pc + 3]) << 24);
				pc += 4;
				// remove the following line once you have a complete simulator
			}
			else {	//compressed instruction
				instWord_comp = memory[pc] | ((memory[pc + 1]) << 8);
				pc += 2;
				instWord = decompress(instWord_comp);
			}

			instDecExec(instWord);
			if (pc > fsize) break;			// stop when PC reached address 32
		}
	}
	else emitError("Cannot access input file\n");
	return 0;
}