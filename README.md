# Assembly_project1
<h3>contributers :</h3> <ul>
  <li>Andrew Aziz</li>
  <li>Kareem Sayed</li>
  <li>Fikry Mohammed</li>
  <li>Mohammad Alashkar</li>
</ul>
<!DOCTYPE html>
<html>
<body>
  <h1>Simulator Instructions</h1>
  <ol>
    <li>Open the computer terminal or command prompt.</li>
    <li>Navigate to the directory where the code file is located using the "cd" command.</li>
    <li>Compile the code file using the C++ compiler. Use the following command:<br>
      <code>g++ -o rvsim final.cpp</code></li>
    <li>Once the compilation is successful, execute the simulator by running the generated output file. Use the following command:<br>
      <code>./rvsim &lt;machine_code_file_name&gt;</code><br>
      Replace <code>&lt;machine_code_file_name&gt;</code> with the name of the machine code file you want to simulate.</li>
    <li>After executing the simulator, it will prompt you to enter the name of the ".data" file. Make sure the ".data" file is located in the same directory as the simulator executable.</li>
    <li>Provide the name of the ".data" file as prompted, and the simulation will start automatically.</li>
  </ol>

  <h1>Simulator Design</h1>
  <h2>Used Data Structures:</h2>
  <ul>
    <li><code>unsigned int pc = 0 </code>: This unsigned int global variable represents the 32-bit program counter register.</li>
    <li><code>unsigned char memory[(16 + 64) * 1024]</code>: This unsigned char global array represents the whole memory.</li>
    <li><code>const unsigned int reg0</code>: This unsigned int global variable represents the hardwired (zero) 32-bit register.</li>
    <li><code>unsigned int reg[31]</code>: this unsigned int global array represents the rest of the 32 registers of the computer (31 registers each one is 32-bits) for example reg[0] represent x1 and so on. 
.</li>
  </ul>

  <h2>Used Functions:</h2>
  <ul>
    <li><code>void R_Type(unsigned int instWord)</code>: Processes instructions of the R-Type format and disassembles the corresponding assembly instruction. Performs arithmetic, logical, and shift operations based on the opcode, funct3, and funct7 fields of the instruction.In addition, It disassembles and prints the corresponding assembly instruction for each operation. Here's a breakdown of the code:
The function declares and initializes several variables to store different components of the instruction, such as rd (destination register), rs1 (source register 1), rs2 (source register 2), funct3 (function 3), funct7 (function 7), opcode, and various immediate values.
The function first extracts the relevant fields from the instruction word using bitwise operations and bit masks. It then uses a switch statement based on the funct3 field to handle different instruction types. Within each case, specific operations are performed based on the opcode and funct7 values.
The code prints the corresponding assembly instruction for each operation, such as ADD, SUB, SLL, SLT, SLTU, XOR, SRA, SRL, OR, and AND, along with its execution.

Overall, this function is responsible for disassembling and executing R-Type instructions, performing arithmetic, logical, and shift operations, and updating the register values accordingly.
.</li><br>
    <li><code>void I_type(unsigned int instWord)</code>: The function takes an instruction word (instWord) as input and performs various operations based on the opcode, funct3, and funct7 fields of the instruction. It disassembles and prints the corresponding assembly instruction for each I_type operation. Here's a breakdown of the code: 

The function declares and initializes several variables to store different components of the instruction, such as rd (destination register), rs1 (source register 1), rs2 (source register 2), funct3 (function 3), funct7 (function 7), opcode, and various immediate values. The function extracts the opcode and other relevant fields from the instruction word using bitwise operations and bit masks. It then proceeds to handle different types of I-Type instructions using a switch statement based on the funct3 field. Within each case, specific operations are performed based on the opcode, funct7, and immediate values. The code prints the corresponding assembly instruction for each operation, such as ADDI, SLLI, SLTI, SLTIU, XORI, SRLI, SRAI, ORI, and ANDI, along with the registers and immediate values involved. The operations update the values of the destination register (rd) based on the results of the calculations( execution). 
Additionally, the code handles specific load instructions (LB and LH) by fetching data from memory based on the immediate value and the source register, and extending the result appropriately.
Overall, this function is responsible for executing I-Type instructions, performing arithmetic, logical, and load operations, and updating the register values accordingly.
.</li><br>
    <li><code>void S_Type(unsigned int instWord)</code>:This function is very similar to the previous function, but it performs the disassembling and execution for the S_Type RV-32i instructions.
</li><br>
    <li><code>void B_Type(unsigned int instWord)</code>: This function is very similar to the previous function, but it performs the disassembling and execution for the B_Type RV-32i instructions ( BEQ, BNE, BLT, BGE, BLTU, BGEU)
</li><br>
    <li><code>void J_Type(unsigned int instWord)</code>: This function is very similar to the previous function, but it performs the disassembling and execution. for the J_Type RV-32i instructions . Note that its execution updates the program counter according to the passed immediate ( JAL).
</li><br>
    <li><code>void U_Type(unsigned int instWord)</code>: This function is very similar to the previous function, but it performs the disassembling and execution for the U_Type RV-32i instructions .(LUI or AUIPC)
</li><br>
    <li><code>void instDecExec(unsigned int instWord)</code>: The main job of this function is to find the opcode of the instruction and then decide which type is this instruction(switch case)(I,S, R… ) then it prints the address of the instruction along with the output of the target function (I_type, S_type….)
.</```html><br>
      <li><code>void instDecExec(unsigned int instWord)</code>: Finds the opcode of the instruction, determines the type of the instruction, and calls the respective function for execution and disassembling.</li>
  </ul>

  <h2>Compressed Handling Functions:</h2>
  <ul>
    <li><code>unsigned int decompress(unsigned short compressed_word)</code>: This function is the entry point for decompression. It takes a compressed 2 bytes instruction as input and returns the decompressed 4 bytes instruction as an unsigned integer. 
It extracts the opcode from the compressed word and calls the corresponding decompression function based on the opcode.
If the opcode is not recognized, it prints an error message and returns 0, which is going to be evaluated as an unknown instruction.</li><br>
    <li><code>unsigned int decompress0(unsigned short compressed_word)</code>: This function handles the decompression of compressed instruction with opcode ‘00’.
It extracts the necessary fields from the compressed word to reconstruct the decompressed instruction.
Depending on the funct3C field, it decompresses different instructions, such as C.LW (load word) and C.SW (store word).
The compressed instruction is printed, and the decompressed instruction is constructed and returned as an unsigned integer.</li><br>
    <li><code>unsigned int decompress1(unsigned short compressed_word)</code>: This function handles the decompression of compressed instruction with opcode ‘01’.
It extracts the necessary fields from the compressed word to reconstruct the decompressed instruction.
Depending on the funct6C and funct2C fields, it decompresses different instructions, such as C.SUB (subtract) and C.XOR (bitwise XOR).
The compressed instruction is printed, and the decompressed instruction is constructed and returned as an unsigned integer.
</li><br>
    <li><code>unsigned int decompress2(unsigned short compressed_word)</code>: This function handles the decompression of compressed instruction with opcode ‘10’.
It extracts various fields from the compressed word, such as uimm1, uimm2, funct3C, opC, rs2C, rs1C, funct4C, and nzimm_5.
It then decompresses different instructions based on the extracted fields, including C.JR (jump register), C.MV (move), C.JALR (jump and link register), C.ADD (add), and C.SLLI (left shift immediate).
The compressed instruction is printed, and the decompressed instruction is constructed and returned as an unsigned integer.
</li>
  </ul>

  <h2>Challenges:</h2>
  <ul>
    <li>We faced a challenge in modifying the pc for the compressed instructions. The idea is that for the compressed instructions, we increase the pc by 2. However,  for the uncompressed, we increase pc by 4. Thus, when we decompress some compressed functions such as c.jal,c.j,c.beqz, and c.bnez and then they get executed by the uncompressed execution, the pc will not be changed correctly. 
Thus, we solved this problem by using a bool variable called indicator which indicates if this instruction is compressed or not. This indicator will be true when it enters the compressed functions. If compressed, we increase the pc by 2 in the uncompressed functions so that in total, pc will be equal (pc-2)+immediate.
</li>
    <li>We also faced another challenge in searching for formats for compressed functions. We find a lot of different versions, and in each version, there were different formats. Finally, we decided to choose the most recent version.</li>
    <li>We also faced another challenge when getting immediate in a certain format for certain instructions because we need to rearrange them and then print them. 
</li>
  </ul>
</body>
</html>
