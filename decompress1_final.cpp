
#include <iostream>	
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[31];
const unsigned int reg0 = 0;

//compressed functions with opcpde '01'
unsigned int decompress1(unsigned short compressed_word)
{
	unsigned int instword;
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int opC ,rs1C ,rs2C, funct2C, rdC, funct6C;
	unsigned int I_imm;
	unsigned int funct3C, shamt;
	unsigned int nzimm4_0;
	unsigned int nzimm_5;
	unsigned int  funct2C_2, rdC_5bits;
	unsigned int lui_imm1,lui_imm2,lui_imm3,lui_imm;
	
	opC = (compressed_word) & 0x00000003;
	rs2C = (compressed_word >> 2) & 0x00000007;
	funct2C = (compressed_word >> 5) & 0x00000003;
	rdC = (compressed_word >> 7) & 0x00000007;
	funct6C = (compressed_word >> 10) & 0x0000003F;
	funct3C = (compressed_word >> 13) & 0x00000007;
	nzimm4_0= (compressed_word >> 2) & 0x0000001f;
	rdC_5bits = (compressed_word >> 7) & 0x0000001f;
	funct2C_2= (compressed_word >> 10) & 0x00000003;
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
			break;
		default:
			cout << "\tUnkown Compressed Instruction \n";
			//intialize the fields with zeros to ensures that it is not mapped to an instruction in RV32I
			opcode = 0;
			funct3 = 0;
			funct7 = 0;
			rs2 = 0;
			rd = 0;
			rs1 = 0;
			instword = funct7;
			instword = (instword << 5) + rs2;
			instword = (instword << 5) + rs1;
			instword = (instword << 3) + funct3;
			instword = (instword << 5) + rd;
			instword = (instword << 7) + opcode;
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

				rd = rdC+8;
				rs1 = rdC+8;
				shamt = nzimm4_0;
				instword = 0x00000000;
				instword = (instword << 5) | shamt;
				instword = (instword << 5) | rs1;
				instword = (instword << 3) | funct3;
				instword = (instword << 5) | rd;
				instword = (instword << 7) | opcode;
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
			}
		}
		else if(funct2C_2 =0b10){ //ANDI
		    
		    opcode = 0x0010011;
		    funct3 = 0x7;
		    lui_imm1 = (compressed_word>>12) & 0x1; //bit number [5]
		    lui_imm2 = (compressed_word>>2) &0x1f; //bit [4:0]
		    rs1C = (compressed_word>>7)&0x7;
		    rdC = rs1C;
		    instword = (lui_imm1); //bit[5]
			instword = (instword<<5) + lui_imm2; //bit[4:0]
			instword = (instword<<5) + rs1C;
			instword = (instword<<3) + funct3;
			instword = (instword<<5) + rdC;
			instword = (instword<<7) + opcode;
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
			add16_imm = add16_imm | (nzimm_5) << 5);
			I_imm = (add16_imm) | ((nzimm_5) ? 0x00000fc0 : 0x00000000);
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


		}
		else
		{
			funct3C = (compressed_word>>13);
			lui_imm1 = (compressed_word>>12)&0x1; //bit [17]
			lui_imm2 = (compressed_word>>2)&0x1f; //bit[16:12]
			rdC = (compressed_word>>7)&0x1f; 
			if(funct3C == 2){//c.li to addi 
			    opcode = 0b0010011;
			    funct3 = 0x0;
			    instword = (lui_imm1); //bit[5]
			    instword = (instword<<5) + lui_imm2; //bit[4:0]
			    instword = (instword<<5) + 0;
			    instword = (instword<<3) + funct3;
			    instword = (instword<<5) + rdC;
			    instword = (instword<<7) + opcode;
			    
			}
			else if(funct3C == 3){ // c.lui 
			    opcode = 0b0110111;
			    instword = (lui_imm1);
			    instword = (instword<<5) + lui_imm2;
			    instword = (instword<<5) + rdC;
			    instword = (instword<<7) + opcode;
			}
			
		}
	}
	else 
    {
		cout << "\tUnkown Compressed Instruction \n";

	}
	//insert the if else for different instruction with opcode =01, but with different func6C or funct3C here
	return instword;

}

unsigned int compressed(unsigned short compressed_word) {

	unsigned int opcode, instword = 0;
	opcode = compressed_word & 0x00000003;
	if (opcode == 0x0)
		instword = decompress0(compressed_word);
	return instword;
}

int main(int argc, char* argv[]) {

	unsigned short instWord = 0xccc8;
	cout << hex << "0x" << (int)decompress0(instWord) << endl;

	return 0;
		
}