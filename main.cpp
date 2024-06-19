#include <bits/stdc++.h>
#include "Decoded_Ins.h"
#include "Decoded_Ins.cpp"
#include "Executed_Ins.h"
#include "Executed_Ins.cpp"
using namespace std;
unsigned short int PC = 0;
Decoded_Ins IF;             // Instruction in Fetch Stage
Decoded_Ins ID;             // Instruction in Decode Stage
Executed_Ins EX;            // Instruction in Execute Stage
Executed_Ins MEM;           // Instruction in Memory Stage
Executed_Ins WB;            // Instruction in Write Back Stage
unsigned short int LMD = 0; // LMD stores the data to be loaded from DCache in case of Load instructions
unsigned short int instructions_executed = 0;
unsigned short int Arithmetic_instructions = 0;
unsigned short int Logical_instructions = 0;
unsigned short int Load_imm_instructions = 0;
unsigned short int Shift_instructions = 0;
unsigned short int Memory_instructions = 0;
unsigned short int Control_instructions = 0;
unsigned short int Halt_instructions = 0;
unsigned short int RAW = 0; // Counter for the number of Read After Write (RAW) stalls
unsigned short int control_stalls = 0;
unsigned short int cycles = 0;
unsigned short int branch_ins_executed = 0;
bool data_stalls_present = false; // If data_stalls_present is false , then there is no RAW hazard , else it's there and don't fetch new instruction.
bool can_fetch = true;            // If can_fetch is true , then there is no control hazard , else it's there and don't fetch new instruction.
bool can_decode = false;          // If can_decode is false , then don't perform instruction decode.
bool can_execute = false;         // If can_execute is false , then don't perform execute stage.
bool can_preform_memory = false;  // If can_preform_memory is false , then don't perform memory operation.
bool can_write_back = false;      // If can_write_back is false , then don't perform write back operation.
vector<unsigned short int> DCache(256, 0);
vector<unsigned short int> ICache(256, 0);
vector<unsigned short int> RegisterFile(16, 0);
vector<unsigned short int> datahazard(16, 0);

void read_input(vector<unsigned short int> &vec, unsigned short int N, string path)
{
    ifstream ifile;
    ifile.open(path);
    for (unsigned short int i = 0; i < N; i++)
    {
        ifile >> hex >> vec[i];
    }
}

void stats(unsigned short int opcode)
{
    instructions_executed++;

    if (opcode == 0b1111)
    {
        Halt_instructions++;
    }
    else if (opcode == 0b1101 || opcode == 0b1110)
    {
        Control_instructions++;
    }
    else if (opcode == 0b1011 || opcode == 0b1100)
    {
        Memory_instructions++;
    }
    else if (opcode == 0b1010)
    {
        Load_imm_instructions++;
    }
    else if (opcode == 0b1000 || opcode == 0b1001)
    {
        Shift_instructions++;
    }
    else if (opcode > 0b0011 && opcode <= 0b0111)
    {
        Logical_instructions++;
    }
    else
    {
        Arithmetic_instructions++;
    }
}

void print_stats(string path)
{
    ofstream ofile;
    ofile.open(path);
    ofile << "Total number of instructions executed:" << instructions_executed << endl;
    ofile << "Number of instructions in each class" << endl;
    ofile << "Arithmetic instructions              :" << Arithmetic_instructions << endl;
    ofile << "Logical instructions                 :" << Logical_instructions << endl;
    ofile << "Shift instructions                   :" << Shift_instructions << endl;
    ofile << "Memory instructions                  :" << Memory_instructions << endl;
    ofile << "Load immediate instructions          :" << Load_imm_instructions << endl;
    ofile << "Control instructions                 :" << Control_instructions << endl;
    ofile << "Halt instructions                    :" << Halt_instructions << endl;
    ofile << "Cycles Per Instruction               :" << (float)cycles / instructions_executed << endl;
    ofile << "Total number of stalls               :" << RAW + control_stalls << endl;
    ofile << "Data stalls  (RAW)                   :" << RAW << endl;
    ofile << "Control stalls                       :" << control_stalls << endl;
}

void update_DCache(string path, int N)
{
    ofstream dofile;
    dofile.open(path);
    for (unsigned short int i = 0; i < N; i++)
    {
        if (DCache[i] >= 16)
            dofile << hex << DCache[i] << endl;
        else
            dofile << hex << 0 << DCache[i] << endl;
    }
}

void Write_Back()
{
    unsigned short int destination_register = (unsigned short int)(WB.instruction << 4) >> 12;
    stats(WB.opcode);

    if (WB.opcode == 0b1011)
    {
        RegisterFile[destination_register] = LMD;
    }
    else if (WB.opcode == 0b1111)
    {
        ;
    }
    else if (WB.opcode != 0b1100 && WB.opcode != 0b1101 && WB.opcode != 0b1110)
    {
        RegisterFile[destination_register] = WB.ALUOutput;
    }
    unsigned short int destination_reg = 16;
    if (WB.opcode <= 0b1011)
        destination_reg = ((unsigned short int)(WB.instruction << 4) >> 12);
    if (destination_reg != 16)
        datahazard[destination_reg]--;
}

void Memory()
{
    if (MEM.opcode == 0b1011)
    {
        LMD = DCache[MEM.ALUOutput];
    }
    else if (MEM.opcode == 0b1100)
    {
        DCache[MEM.ALUOutput] = MEM.B;
    }
}

void Decode()
{
    ID.decode_instruction();
    if (ID.data_hazard_present(datahazard))
    {
        data_stalls_present = true;
    }
    else
    {
        ID.register_read(RegisterFile);
        data_stalls_present = false;
        PC += 2;
        if (ID.opcode <= 0b1011)
        {
            datahazard[ID.destination_register]++;
        }
    }
    if (ID.opcode == 0b1111)
        can_fetch = false;
    if (ID.opcode == 0b1101 || ID.opcode == 0b1110 && !data_stalls_present)
    {
        can_fetch = false;
    }
}

void Fetch()
{
    if (!data_stalls_present)
    {
        unsigned short int IR = ICache[PC];
        IR = (unsigned short int)(IR << 8);
        IR |= ICache[PC + 1];
        IF.instruction = IR;
    }
    else
    {
        RAW++;
    }
}

int32_t main()
{
    // load the Data in datacache DCache
    read_input(DCache, 256, "input/DCache.txt");

    // load the instructions in instructioncache ICache
    read_input(ICache, 256, "input/ICache.txt");

    // load the registers in RegisterFile
    read_input(RegisterFile, 16, "input/RF.txt");

    while (1)
    {
        cycles++;
        if (can_write_back)
        {
            Write_Back();
            if (WB.opcode == 0b1111)
                break;
        }
        if (can_preform_memory)
        {
            Memory();
        }
        if (can_execute)
        {
            EX.execute(branch_ins_executed, PC);
        }
        if (can_decode)
        {
            Decode();
        }
        if (can_fetch)
        {
            Fetch();
        }
        else if ((IF.instruction >> 12) != 0b1111)
        {
            control_stalls++;
        }
        WB = MEM;
        MEM = EX;
        can_write_back = can_preform_memory;
        can_preform_memory = can_execute;
        if (!data_stalls_present)
        {
            EX.instruction = ID.instruction;
            EX.A = ID.A;
            EX.B = ID.B;
            EX.opcode = ID.opcode;
            can_execute = can_decode;
            ID = IF;
            can_decode = can_fetch;
        }
        else if (data_stalls_present)
        {
            can_execute = false;
        }
        if (branch_ins_executed == 1)
        {
            can_fetch = true;
            branch_ins_executed = 0;
        }
    }

    update_DCache("output/DCache.txt", 256);
    print_stats("output/Output.txt");
}