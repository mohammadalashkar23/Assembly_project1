#include <iostream>

using namespace std;

void S_Type(unsigned int instWord) {
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
    unsigned int address;

   // unsigned int instPC = pc - 4;

    opcode = instWord & 0x0000007F;
    rd = (instWord >> 7) & 0x0000001F;
    funct3 = (instWord >> 12) & 0x00000007;
    rs1 = (instWord >> 15) & 0x0000001F;
    rs2 = (instWord >> 20) & 0x0000001F;

    unsigned int temp = ((instWord >> 25) & 0x3F);
    temp = temp << 5;
    S_imm = ((instWord >> 7) & 0x1F) | temp | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

    switch (funct3) {
        case 0:
            cout << "\tSB\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
            break;
        case 1:
            cout << "\tSH\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
            break;
        case 2:
            cout << "\tSW\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
            break;
    }
}

int main() {
    unsigned int instWord = 0b00000000111000010010010000100011;
    S_Type(instWord);
    return 0;
}
