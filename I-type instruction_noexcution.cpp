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

	// — inst[31] — inst[30:25] inst[24:21] inst[20]
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

	unsigned shamt;
	shamt = I_imm & 0x01F;
	funct7 = (I_imm >> 5) & 0x07F;

	if (opcode == 0x13) {	// I instructions
		switch (funct3) {
		case 0:	cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			
			break;
		case 1:	cout << "\tSLLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << shamt << "\n";
			
			break;
		case 2:	cout << "\tSLTI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			
			break;
		case 3:	cout << "\tSLTIU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			
			break;
		case 4:	cout << "\tXORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			
			break;
		case 5:
			switch (funct7) {
			case 0:	cout << "\tSRLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << shamt << "\n";
				
				break;
			case 32:	cout << "\tSRAI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << shamt << "\n";
				
				break;
			default:
				cout << "\tUnkown I Instruction \n";
			}
			break;
		case 6:	cout << "\tORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			
			break;
		case 7:	cout << "\tANDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			
			break;
		default:
			cout << "\tUnkown I Instruction \n";
		}
	}
	else {
		switch (funct3) {
		case 0:	cout << "\tLB\tx" << rd << ", " << (int)I_imm << " (x" << rs1 << ')' << "\n";
			
			break;
		case 1:	cout << "\tLH\tx" << rd << ", " << (int)I_imm << " (x" << rs1 << ')' << "\n";
			
			break;
		case 2:	cout << "\tLW\tx" << rd << ", " << (int)I_imm << " (x" << rs1 << ')' << "\n";
			
			break;
		case 4:	cout << "\tLBU\tx" << rd << ", " << (int)I_imm << " (x" << rs1 << ')' << "\n";
			
			break;
		case 5:	cout << "\tLHU\tx" << rd << ", " << (int)I_imm << " (x" << rs1 << ')' << "\n";
			
			break;
		default:
			cout << "\tUnkown I_load Instruction \n";
		}
	}
}