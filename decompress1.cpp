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

//compressed functions with opcpde '01'
unsigned int decompress1(unsigned short compressed_word)
{
	unsigned int instword;
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned short opC, rs2C, funct2C, rdC, funct6C;
	unsigned int instPC = pc - 4;

	opC = (compressed_word) & 0x00000003;
	rs2C = (compressed_word >> 2) & 0x00000007;
	funct2C= (compressed_word >> 5) & 0x00000003;
	rdC = (compressed_word >> 7) & 0x00000007;
	funct6C = (compressed_word >> 10) & 0x0000003F;

	if(funct6C== 0b100011){
		switch(funct2C){
			case 0: //sub
			opcode= 0b0110011;
			funct3= 0x0;
			funct7= 0x20;
			rs2= rs2C + 8;
			rd= rdC+8;
			rs1= rd;
			break;

			case 1: // Xor
			opcode= 0b0110011;
			funct3= 0x4;
			funct7= 0x00;
			rs2= rs2C + 8;
			rd= rdC+8;
			rs1= rd;
			break;

			case 2:	//Or
			opcode= 0b0110011;
			funct3= 0x6;
			funct7= 0x00;
			rs2= rs2C + 8;
			rd= rdC+8;
			rs1= rd;
			break;

			case 3:	//And
			opcode= 0b0110011;
			funct3= 0x7;
			funct7= 0x00;
			rs2= rs2C + 8;
			rd= rdC+8;
			rs1= rd;
			break;
			default:
			cout << "\tUnkown Compressed Instruction \n";
			//intialize the fields with zeros to ensures that it is not mapped to an instruction in RV32I
			opcode= 0;
			funct3= 0;
			funct7= 0;
			rs2= 0;
			rd= 0;
			rs1= 0;
		}
	}
	//insert the if else for different instruction with opcode =01, but with different func6C or funct3C here
	else {
		cout << "\tUnkown Compressed Instruction \n";

	}
	instword = funct7;
	instword = (instword <<5) + rs2;
	instword = (instword <<5) + rs1;
	instword = (instword <<3) + funct3;
	instword = (instword <<5) + rd;
	instword = (instword <<7) + opcode;
	return instword;
	
	}

