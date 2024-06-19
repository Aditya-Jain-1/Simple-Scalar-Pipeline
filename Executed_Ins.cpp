#include "Executed_Ins.h"
void Executed_Ins::execute(unsigned short int &branch_ins_executed, unsigned short int &PC)
{
    if (opcode == 0b1011 || opcode == 0b1100)
    {
        unsigned short int imm = ((unsigned short int)(instruction << 12) >> 12);
        if (imm >= 8)
        {
            imm = (unsigned short int)(imm << 13) >> 13;
            imm -= 8;
        }
        ALUOutput = A + imm;
    }
    else if (opcode == 0b0000)
    {
        ALUOutput = A + B;
    }
    else if (opcode == 0b0001)
    {
        ALUOutput = A - B;
    }
    else if (opcode == 0b0010)
    {
        ALUOutput = A * B;
    }
    else if (opcode == 0b0011)
    {
        ALUOutput = A + 1;
    }
    else if (opcode == 0b0100)
    {
        ALUOutput = (A & B);
    }
    else if (opcode == 0b0101)
    {
        ALUOutput = (A | B);
    }
    else if (opcode == 0b0110)
    {
        ALUOutput = (A ^ B);
    }
    else if (opcode == 0b0111)
    {
        ALUOutput = (~A);
    }
    else if (opcode == 0b1000)
    {
        unsigned short int imm = ((unsigned short int)(instruction << 12) >> 12);
        ALUOutput = (unsigned short int)(A << imm);
    }
    else if (opcode == 0b1001)
    {
        unsigned short int imm = ((unsigned short int)(instruction << 12) >> 12);
        ALUOutput = (A >> imm);
    }
    else if (opcode == 0b1010)
    {
        unsigned short int imm = ((unsigned short int)(instruction << 8) >> 8);
        ALUOutput = imm;
    }
    else if (opcode == 0b1101)
    {
        unsigned short int imm = ((unsigned short int)(instruction << 4) >> 8);
        imm = (unsigned short int)imm << 1;
        ALUOutput = PC + imm;
        ALUOutput = ((unsigned short int)(ALUOutput << 8) >> 8);
        PC = ALUOutput;
        branch_ins_executed = 1;
    }
    else if (opcode == 0b1110)
    {
        unsigned short int imm = ((unsigned short int)(instruction << 8) >> 8);
        imm = (unsigned short int)(imm << 1);
        ALUOutput = PC + imm;
        ALUOutput = ((unsigned short int)(ALUOutput << 8) >> 8);
        branch_ins_executed = 1;
        if (A == 0)
        {
            PC = ALUOutput;
        }
    }
    ALUOutput = ((unsigned short int)(ALUOutput << 8) >> 8);
}