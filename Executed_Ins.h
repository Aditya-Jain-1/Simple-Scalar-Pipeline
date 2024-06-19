#include <bits/stdc++.h>
#include "Instruction.h"
using namespace std;
#pragma once
class Executed_Ins : public Instruction
{
public:
    unsigned short int A;
    unsigned short int B;
    unsigned short int ALUOutput;
    unsigned short int opcode;
    Executed_Ins(unsigned short int ins = 0) : Instruction(ins)
    {
        ;
    }
    void execute(unsigned short int &branch_ins_executed, unsigned short int &PC);
};