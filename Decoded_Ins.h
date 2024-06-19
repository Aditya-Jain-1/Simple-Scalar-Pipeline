#include <bits/stdc++.h>
#include "Instruction.h"
using namespace std;
#pragma once
class Decoded_Ins : public Instruction
{
public:
    unsigned short int opcode;
    unsigned short int destination_register;
    unsigned short int source_register_1;
    unsigned short int source_register_2;
    unsigned short int A;
    unsigned short int B;
    Decoded_Ins(unsigned short int ins = 0) : Instruction(ins)
    {
        ;
    }
    void decode_instruction();
    bool data_hazard_present(vector<unsigned short int> &datahazard);
    void register_read(vector<unsigned short int> &register_file);
};