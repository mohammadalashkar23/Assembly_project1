#include <iostream>
#include <bitset>
using namespace std;

unsigned int pc;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[31];
const  int reg0 = 0;

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
            if(rs1==0 && rs2==0){
                instPC = reg0 + S_imm;
                if(signed(instPC) >=0){
                memory[instPC] = 0;
                cout << "\tSB\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
                }
                else
                cout << "\texecution terminated with errors.\n";
            }
            else if(rs1 !=0 && rs2==0 ){
                instPC = reg[rs1-1] + S_imm;
                if(signed(instPC)>=0){
                memory[instPC] = 0;
                cout << "\tSB\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
                }
                else
                cout << "\texecution terminated with errors.\n";
            }
            else if(rs2!=0 && rs1 ==0 ){
                instPC = reg0 + S_imm;
                if(signed(instPC) >=0){
                memory[instPC] = reg[rs2-1] & 0xFF;
                cout << "\tSB\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
                }
                else
                cout << "\texecution terminated with errors.\n";
            }
            else{
            instPC = reg[rs1-1] + S_imm;
            if (instPC < sizeof(memory) && signed(instPC) >=0){
                memory[instPC] = reg[rs2-1] & 0xFF;
                cout << "\tSB\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";
            }
            else
            return;
            }
        }
            break;
        
        case 1: {
            if(rs2==0 && rs1 ==0 ){
                instPC = reg0 + S_imm;
                if(signed(instPC) >=0){
                memory[instPC] = 0;
                memory[instPC+1]=0;
                cout << "\tSH\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

                }
                else
                cout << "\texecution terminated with errors.\n";
            }
            else if(rs2==0 && rs1 !=0 ){
                instPC = reg[rs1-1] + S_imm;
                if(signed(instPC) >=0){
                memory[instPC] = 0;
                memory[instPC+1] = 0;
                cout << "\tSH\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

                }
                else
                cout << "\texecution terminated with errors.\n";
            }
            else if(rs2!=0 && rs1 ==0 ){
                instPC = reg0 + S_imm;
                if(signed(instPC) >=0){
                memory[instPC] = reg[rs2-1] & 0xFF;
                memory[instPC + 1] = (reg[rs2-1] >> 8) & 0xFF;
                cout << "\tSH\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

                }
                else
                cout << "\texecution terminated with errors.\n";
            }
            else{
            instPC = reg[rs1-1] + S_imm;
            if (instPC < sizeof(memory) - 1 && signed(instPC) >=0){
                memory[instPC] = reg[rs2-1] & 0xFF;
                memory[instPC + 1] = (reg[rs2-1] >> 8) & 0xFF;
                cout << "\tSH\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

            }
            else
                cout << "\texecution terminated with errors.\n";
            }
            break;
        }
        case 2: {
            if(rs2==0 && rs1 ==0 ){
                instPC = reg0 + S_imm;
                 if(signed(instPC) >=0){
                memory[instPC] = 0;
                memory[instPC+1]=0;
                memory[instPC+2]=0;
                memory[instPC+3]=0;
                cout << "\tSW\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

                 }
                 else
                cout << "\texecution terminated with errors.\n";
            }
            else if(rs2==0 && rs1 !=0 ){
                instPC = reg[rs1-1] + S_imm;
                if(signed(instPC) >=0){
                memory[instPC] = 0;
                memory[instPC+1] = 0;
                memory[instPC+2]=0;
                memory[instPC+3]=0;
                cout << "\tSW\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

                }
                else
                cout << "\texecution terminated with errors.\n";
            }
            else if(rs2!=0 && rs1 ==0 ){
                instPC = reg0 + S_imm;
                if(signed(instPC) >=0){
                memory[instPC] = reg[rs2-1] & 0xFF;
                memory[instPC + 1] = (reg[rs2-1] >> 8) & 0xFF;
                memory[instPC + 2] = (reg[rs2-1] >> 16) & 0xFF;
                memory[instPC + 3] = (reg[rs2-1] >> 24) & 0xFF;
                cout << "\tSW\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

                }
                else
                cout << "\texecution terminated with errors.\n";
            }
            else{
            instPC = reg[rs1-1] + S_imm;
            if (instPC < sizeof(memory) - 3 && signed(instPC) >=0) {
                memory[instPC] = reg[rs2-1] & 0xFF;
                memory[instPC + 1] = (reg[rs2-1] >> 8) & 0xFF;
                memory[instPC + 2] = (reg[rs2-1] >> 16) & 0xFF;
                memory[instPC + 3] = (reg[rs2-1] >> 24) & 0xFF;
                cout << "\tSW\tx" << rs2 << ", " << S_imm << "(x" << rs1 << ")\n";

            }
            else
                cout << "\texecution terminated with errors.\n";
            }
            break;
        }
    }
}

int main() {
    unsigned int instWord = 0xfe11a123;
    S_Type(instWord);
    return 0;
}
