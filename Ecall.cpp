#include <iostream>	
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc;
unsigned char memory[(16 + 64) * 1024];
unsigned int reg[31];
const unsigned int reg0 = 0;


void Ecall(unsigned int instWord)
{
	if (reg[16] == 1) //a7
		cout << (int)reg[9] << endl; //a0
	else if (reg[16] == 4) {
		unsigned int i = reg[9]; //i=address
		while (memory[i] != NULL) {
			cout << memory[i];
			++i;
		}
		cout << endl;
	}
	else
		exit(0);
}
int main(int argc, char* argv[]) {
	int instWord = 0x00000073;
	reg[16] = 4;
	reg[9] = 1000;
	memory[1000] = 's';
	memory[1001] = 'y';
	Ecall(instWord);
	return 0;
}