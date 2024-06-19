#include "Decoded_Ins.h"
void Decoded_Ins::decode_instruction()
{
    opcode = ((unsigned short int)(instruction >> 12));
    destination_register = ((unsigned short int)(instruction << 4) >> 12);
    source_register_1 = ((unsigned short int)(instruction << 8) >> 12);
    source_register_2 = ((unsigned short int)(instruction << 12) >> 12);
}

bool Decoded_Ins::data_hazard_present(vector<unsigned short int> &datahazard)
{
    bool hazard = false;
    if (opcode == 0b0011 || opcode == 0b1110)
    {
        hazard |= datahazard[destination_register];
    }
    else if (opcode == 0b1010 || opcode == 0b1111 || opcode == 0b1101)
        ;
    else if (opcode == 0b1011)
    {
        hazard |= datahazard[source_register_1];
    }
    else if (opcode == 0b1100)
    {
        hazard |= datahazard[source_register_1];
        hazard |= datahazard[destination_register];
    }
    else if (opcode == 0b1000 || opcode == 0b1001 || opcode == 0b0111)
    {
        hazard |= datahazard[source_register_1];
    }
    else
    {
        hazard |= datahazard[source_register_1];
        hazard |= datahazard[source_register_2];
    }
    return hazard;
}

void Decoded_Ins::register_read(vector<unsigned short int> &register_file)
{
    if (opcode == 12)
    {
        A = register_file[source_register_1];
        B = register_file[destination_register];
    }
    else if (opcode != 14 && opcode != 3)
    {
        A = register_file[source_register_1];
        B = register_file[source_register_2];
    }
    else
    {
        A = register_file[destination_register];
    }
}