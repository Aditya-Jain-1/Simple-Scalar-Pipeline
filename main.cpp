#include <bits/stdc++.h>
using namespace std;
#define int unsigned short int
int32_t main()
{
    // load the Data in datacache D$
    ifstream dfile;
    dfile.open("DCache.txt");
    vector<int> D$(256, 0);
    for (int i = 0; i < 256; i++)
    {
        dfile >> hex >> D$[i];
    }

    // load the instructions in instructioncache I$
    ifstream ifile;
    ifile.open("ICache.txt");
    vector<int> I$(256, 0);
    for (int i = 0; i < 256; i++)
    {
        int a, b;
        ifile >> hex >> I$[i];
    }

    // load the registers in RF
    ifstream rfile;
    rfile.open("RF.txt");
    vector<int> RF(16, 0);
    for (int i = 0; i < 16; i++)
    {
        rfile >> hex >> RF[i];
    }

    int PC = 0;
    int IF = 0;
    int ID = 0;
    int EX = 0;
    int MEM = 0;
    int WB = 0;
    int LMD = 0;
    int ALUOutput = 0;
    int ALUOutput1 = 0;
    int A = 0;
    int B = 0;
    int B1 = 0;

    vector<int> datahazard(16, 0);
    // If check is false , then there is no RAW hazard , else it's there and don't fetch new instruction.
    bool check = false;
    // If cif is true , then there is no control hazard , else it's there and don't fetch new instruction.
    bool cif = true;
    // If cid is false , then don't perfrom instruction decode.
    bool cid = false;
    // If cex is false , then don't perfrom execute stage.
    bool cex = false;
    // If cmem is false , then don't perfrom memory operation.
    bool cmem = false;
    // If cwb is false , then don't perfrom write back operation.
    bool cwb = false;

    int instructions_executed = 0;
    int Arithmetic_instructions = 0;
    int Logical_instructions = 0;
    int Load_imm_instructions = 0;
    int Shift_instructions = 0;
    int Memory_instructions = 0;
    int Control_instructions = 0;
    int Halt_instructions = 0;
    int RAW = 0;
    int control_stalls = 0;
    int cycles = 0;
    int d = 0;

    while (1)
    {
        cycles++;
        if (cwb)
        {
            int opcode = (WB >> 12);
            int rd = (int)(WB << 4) >> 12;
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

            if (opcode == 0b1011)
            {
                RF[rd] = LMD;
            }
            else if (opcode == 0b1111)
            {
                break;
            }
            else if (opcode != 0b1100 && opcode != 0b1101 && opcode != 0b1110)
            {
                RF[rd] = ALUOutput1;
            }
        }
        if (cmem)
        {
            ALUOutput1 = ALUOutput;
            int opcode = (MEM >> 12);
            if (opcode == 0b1011)
            {
                LMD = D$[ALUOutput];
            }
            else if (opcode == 0b1100)
            {
                D$[ALUOutput] = B1;
            }
            int r1 = 16;
            if (opcode <= 11)
                r1 = ((int)(MEM << 4) >> 12);
            if (r1 != 16)
                datahazard[r1]--;
        }
        if (cex)
        {
            B1 = B;
            int opcode = (EX >> 12);
            if (opcode == 0b1011 || opcode == 0b1100)
            {
                int imm = ((int)(EX << 12) >> 12);
                if (imm >= 8)
                {
                    imm = (int)(imm << 13) >> 13;
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
                int imm = ((int)(EX << 12) >> 12);
                ALUOutput = (int)(A << imm);
            }
            else if (opcode == 0b1001)
            {
                int imm = ((int)(EX << 12) >> 12);
                ALUOutput = (A >> imm);
            }
            else if (opcode == 0b1010)
            {
                int imm = ((int)(EX << 8) >> 8);
                ALUOutput = imm;
            }
            else if (opcode == 0b1101)
            {
                int imm = ((int)(EX << 4) >> 8);
                imm = (int)imm << 1;
                ALUOutput = PC + imm;
                ALUOutput = ((int)(ALUOutput << 8) >> 8);
                PC = ALUOutput;
                d = 1;
            }
            else if (opcode == 0b1110)
            {
                int imm = ((int)(EX << 8) >> 8);
                imm = (int)(imm << 1);
                ALUOutput = PC + imm;
                ALUOutput = ((int)(ALUOutput << 8) >> 8);
                d = 1;
                if (A == 0)
                {
                    PC = ALUOutput;
                }
            }
            else if (opcode == 0b1111)
            {
                ;
            }
            ALUOutput = ((int)(ALUOutput << 8) >> 8);
        }
        if (cid)
        {
            int opcode = (ID >> 12);
            if (opcode == 15)
                cif = false;
            if (opcode == 12)
            {
                int rs1 = ((int)(ID << 8) >> 12);
                int rd = ((int)(ID << 4) >> 12);
                A = RF[rs1];
                B = RF[rd];
            }
            else if (opcode != 14 && opcode != 3)
            {
                int rs1 = ((int)(ID << 8) >> 12);
                int rs2 = ((int)(ID << 12) >> 12);
                A = RF[rs1];
                B = RF[rs2];
            }
            else
            {
                int rs1 = ((int)(ID << 4) >> 12);
                A = RF[rs1];
            }
        }
        if (cif)
        {
            if (!check)
            {
                IF = I$[PC];
                IF = (int)(IF << 8);
                IF |= I$[PC + 1];
            }
            else if (PC >= I$.size())
            {
                IF = d;
            }
            else
            {
                RAW++;
            }
            int opcode = (IF >> 12);
            bool hazard = false;
            if (opcode == 0b0011 || opcode == 0b1110)
            {
                int rs1 = ((int)(IF << 4)) >> 12;
                hazard |= datahazard[rs1];
            }
            else if (opcode == 0b1010 || opcode == 0b1111 || opcode == 0b1101)
                ;
            else if (opcode == 0b1011)
            {
                int rs1 = ((int)(IF << 8)) >> 12;
                hazard |= datahazard[rs1];
            }
            else if (opcode == 0b1100)
            {
                int rs1 = ((int)(IF << 8)) >> 12;
                hazard |= datahazard[rs1];
                int rd = ((int)(IF << 4)) >> 12;
                hazard |= datahazard[rd];
            }
            else if (opcode == 0b1000 || opcode == 0b1001 || opcode == 0b0111)
            {
                int rs1 = ((int)(IF << 8)) >> 12;
                hazard |= datahazard[rs1];
            }
            else
            {
                int rs1 = ((int)(IF << 8)) >> 12;
                hazard |= datahazard[rs1];
                int rs2 = ((int)(IF << 12)) >> 12;
                hazard |= datahazard[rs2];
            }
            if (hazard > 0)
                check = true;
            else
                check = false;
            if (!check)
                PC = PC + 2;
            if (opcode <= 11 && !check)
            {
                int rd = ((int)(IF << 4) >> 12);
                datahazard[rd]++;
            }
        }
        else if ((IF >> 12) != 15)
        {
            control_stalls++;
        }
        WB = MEM;
        MEM = EX;
        EX = ID;
        cwb = cmem;
        cmem = cex;
        cex = cid;
        if (!check)
        {
            ID = IF;
            cid = cif;
        }
        else if (check)
        {
            cid = false;
        }
        int opcode = (IF >> 12);
        if (opcode == 13 || opcode == 14 && !check)
        {
            cif = false;
        }
        if (d == 1)
        {
            cif = true;
            d = 0;
        }
    }

    ofstream dofile;
   // Update the DCache file
    dofile.open("DCache.txt");
    for (int i = 0; i < 256; i++)
    {
        if (D$[i] >= 16)
            dofile << hex << D$[i] << endl;
        else
            dofile << hex << 0 << D$[i] << endl;
    }

    ofstream ofile;
    ofile.open("Output.txt");
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
