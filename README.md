## Simple Scalar Pipelined Processor 
   
### Objective
To understand the working of a simple scalar pipelined processor by simulating the different components at the software level and determining the stalls produced due to branch instructions and dependencies among different instructions.

### Description
* The Pipeline has 5 stages, namely, Instruction Fetch (IF), Instruction Decode (ID), Execute (EX), Memory (MEM) and Write Back(WB).
* Instruction Fetch: In this stage, instruction is fetched from ICache.
* Decode: In this stage, the source and destination registers and the operation to be performed are identified and data of source registers is read into two temporary registers A and B, also the dependencies are indentified if any.
* Execute: ALU performs the corresponding operation and the result is stored in temporary register ALUOutput.
* Memory: In case of Load and Store instructions, the data is loaded (into LMD) or stored in DCache respectively.
* Write Back Stage: The destination register is updated with the value of ALUOutput in case of non-memory instructions and LMD in case of a Load instruction.
* Data Hazards are handled by keeping a track of the destination registers whose values aren't updated/written back till now through the vector datahazard.
* Control Hazards are handled using the can_fetch variable which is set to false whenever there is a Branch or Jump instruction and accordingly the pipeline in stalled.
  
### How to run the code
 1. Compile the cpp file using:
           `g++ main.cpp`
 2. After Compiling, use the command : `.\a.exe` <br>
