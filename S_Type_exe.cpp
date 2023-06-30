#include <iostream>
#include <bitset>
using namespace std;

unsigned int pc;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[31];
const unsigned int reg0 = 0;

void S_Type(unsigned int instWord) {
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
    unsigned int address;

    unsigned int instPC;

    opcode = instWord & 0x0000007F;
    rd = (instWord >> 7) & 0x0000001F;
    funct3 = (instWord >> 12) & 0x00000007;
    rs1 = (instWord >> 15) & 0x0000001F;
    rs2 = (instWord >> 20) & 0x0000001F;

    unsigned int temp = ((instWord >> 25) & 0x3F);
    S_imm = ((instWord >> 7) & 0x1F) | (temp << 5) | (((instWord >> 31) & 0x1) ? 0xFFFFF800 : 0x0);
    
    reg[rs1-1] = 10004; //for testing
    reg[rs2-1] = 0b01010101010101010101010101010101;  //for testing
    
    switch (funct3) {
        case 0: {
            cout << "\tSB\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
            instPC = reg[rs1-1] + S_imm;
            if (instPC < sizeof(memory))
                memory[instPC] = reg[rs2-1] & 0xFF;
            cout << bitset<8>(memory[instPC]); //for testing
            break;
        }
        case 1: {
            cout << "\tSH\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
            instPC = reg[rs1-1] + S_imm;
            if (instPC < sizeof(memory) - 1)
                memory[instPC] = reg[rs2-1] & 0xFF;
            if (instPC + 1 < sizeof(memory))
                memory[instPC + 1] = (reg[rs2-1] >> 8) & 0xFF;
            break;
        }
        case 2: {
            cout << "\tSW\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
            instPC = reg[rs1-1] + S_imm;
            if (instPC < sizeof(memory) - 3) {
                memory[instPC] = reg[rs2-1] & 0xFF;
                memory[instPC + 1] = (reg[rs2-1] >> 8) & 0xFF;
                memory[instPC + 2] = (reg[rs2-1] >> 16) & 0xFF;
                memory[instPC + 3] = (reg[rs2-1] >> 24) & 0xFF;
            }
            break;
        }
    }
}

int main() {
    unsigned int instWord = 0b00000000111000010000010000100011;
    S_Type(instWord);
    return 0;
}
