#include <iostream>	
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[31];
const unsigned int reg0 = 0;

//compressed functions with opcpde '10'
unsigned int decompress2(unsigned short compressed_word)
{
	unsigned int instword;
	unsigned int rd, rs1, rs2, funct3, funct7, opcode, uimm1,uimm2, uimm;
	unsigned short opC, rs2C , rs1C ,funct3C ,funct4C;
	unsigned int instPC = pc - 4;

	opC = (compressed_word) & 0x00000003;
	rs2C = (compressed_word >> 2) & 0x0000001f;
	rs1C= (compressed_word >> 7) & 0x0000001f;
	funct4C = (compressed_word >> 12) & 0x0000000f;
	funct3C = (compressed_word>>13);

	if(funct4C== 0b1000){ // C.JR and C.MV
		
		if(rs2C==0){	// JR
			opcode= 0b1100111;
			funct3= 0x0;
			funct7= 0x00;
			rs2= 0x00;
			rd= 0x00;
			rs1= rs1C;
			instword = funct7;
			instword = (instword <<5) + rs2;
			instword = (instword <<5) + rs1;
			instword = (instword <<3) + funct3;
			instword = (instword <<5) + rd;
			instword = (instword <<7) + opcode;
		}

		else{	// MV
			opcode= 0b0110011;
			funct3= 0x0;
			funct7= 0x00;
			rs2= rs2C;
			rd= rs1C;
			rs1= 0x00;
			instword = funct7;
			instword = (instword <<5) + rs2;
			instword = (instword <<5) + rs1;
			instword = (instword <<3) + funct3;
			instword = (instword <<5) + rd;
			instword = (instword <<7) + opcode;
		}
	
	}

	else if(funct4C== 0b1001){ // C.Jalr and C.Add
		
		if(rs2C==0){	// Jalr
			opcode= 0b1100111;
			funct3= 0x0;
			funct7= 0x00;
			rs2= 0x00;
			rd= 0x01;
			rs1= rs1C;
			instword = funct7;
			instword = (instword <<5) + rs2;
			instword = (instword <<5) + rs1;
			instword = (instword <<3) + funct3;
			instword = (instword <<5) + rd;
			instword = (instword <<7) + opcode;
		}

		else{	// Add
			opcode= 0b0110011;
			funct3= 0x0;
			funct7= 0x00;
			rs2= rs2C;
			rd= rs1C;
			rs1= rs1C;
			instword = funct7;
			instword = (instword <<5) + rs2;
			instword = (instword <<5) + rs1;
			instword = (instword <<3) + funct3;
			instword = (instword <<5) + rd;
			instword = (instword <<7) + opcode;
		}
	}
	else if(funct3C == 6){ //swsp
	    uimm1 = (compressed_word>>7)& 0x3; //bits [7:6]
	    uimm2 = (compressed_word>>9)&0xf; //bits [5:2]
	    uimm  = (uimm1<<7)+(uimm2<<2); //765,43200
	    
	    opcode = 0b0100011;
	    funct3 = 0x2;
	    rs2C = (compressed_word>>2)&0x1f;
	    
	    instword = (uimm>>5);
		instword = (instword <<5) + rs2C;
		instword = (instword <<5) + 2;
		instword = (instword <<3) + funct3;
		instword = (instword <<5) + (u_imm&0x1f);
		instword = (instword <<7) + opcode;
	}

	else {
		cout << "\tUnkown Compressed Instruction \n";
		opcode= 0x00;
		funct3= 0x0;
		funct7= 0x00;
		rs2= 0x00;
		rd= 0x00;
		rs1= 0x00;
		instword = funct7;
		instword = (instword <<5) + rs2;
		instword = (instword <<5) + rs1;
		instword = (instword <<3) + funct3;
		instword = (instword <<5) + rd;
		instword = (instword <<7) + opcode;

	}
	

	return instword;
	
	}
