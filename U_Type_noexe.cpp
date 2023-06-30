#include <iostream>

using namespace std;

void U_Type(unsigned int instWord) {
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
    unsigned int address;

   // unsigned int instPC = pc - 4;

    opcode = instWord & 0x0000007F;
    rd = (instWord >> 7) & 0x0000001F;
    funct3 = (instWord >> 12) & 0x00000007;
    rs1 = (instWord >> 15) & 0x0000001F;
    rs2 = (instWord >> 20) & 0x0000001F;

    U_imm = ((instWord >> 12) & 0x7FFFF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

    switch (opcode) {
        case 55:
            cout << "\tLUI\tx" << rd << ", 0x" << std::hex<< U_imm << "\n";
            break;
        case 23:
            cout << "\tAUIPC\tx" << rd << ", 0x" << std::hex<< U_imm << "\n";
            break;
    }
}

int main() {
    unsigned int instWord = 0b00000000111000010010010000110111;
    U_Type(instWord);
    return 0;
}
