#include "chip8.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

void Chip8::loadGame(std::string filename) {
    std::fstream* file = new std::fstream(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        exit(1);
    }
    file->seekg(0, std::ios::end);
    std::streampos fileSize = file->tellg();
    file->seekg(0, std::ios::beg);

    if (fileSize == -1) {
        std::cerr << "Failed to get file size" <<  std::endl;
        file->close();
        exit(1);
    }
    this->file = file;
    this->fileSize = fileSize;
}

void Chip8::initialize(){
    // Initialize registers and memory once
    this->pc == 0x200;
    this->opcode = 0;
    this->I = 0;
    this->sp = 0;

    //clear display

    //clear stack

    //clear registers V0 - VF

    //clear memory

    //load font set
    this->fontset = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, 
        0x20, 0x60, 0x20, 0x20, 0x70, 
        0xF0, 0x10, 0xF0, 0x80, 0xF0, 
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
        0x90, 0x90, 0xF0, 0x10, 0x10,
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
        0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x80, 0xF0,
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        0xF0, 0x80, 0xF0, 0x80, 0x80,
        };
    for (int i = 0; i < 80; i++) {
        this->memory[i] = this->fontset[i];
    }

    this->file.read(this->memory, this->fileSize);

    //reset timers
    this->delayTimer = 0;
    this->soundTimer = 0;
}

void chip8::emulateCycle(){
    // Fetch opcode
    this->opcode = memory[pc] << 8 | memory[pc+1];
    // Decode opcode -- 35 opcodes :)))))))
    switch(this->opcode & 0xF000) {

        case 0x0000:
            switch(this->opcode & 0x00FF) {
                case 0x00E0: //clear screen
                    break;
                case 0x00EE: //Return from subroutine. Set PC to value at top of stack. Decrement SP.
                    break;
                default: //unknown command
                    return;
            }
        case 0x1000: //Jump to location NNN. Set PC to NNN.
            this->pc = this->opcode & 0x0FFF; //ERROR SUSPECT!!!!!!!!!!!!!!!!!!
            break; 

        case 0x2000: //Call subroutine at NNN. Increment SP, store current PC at top of stack. Set PC to NNN.
            this->sp++;
            this->stack[sp-1] = this->pc;
            this->pc = this->opcode & 0x0FFF; //ERROR SUSPECT!!!!!!!!!!!!!!!!!!!!!!!!!
            break;

        case 0x3000: //Opcode format: 3xkk. Skip next instruction if Vx == kk. Compare register Vx and kk. If equal, increment PC by 4. Every instruction occupies 2 bytes.
            if (this->V[(this->opcode & 0x0F00) >> 8] == this->opcode & 0x00FF) {
                this->pc += 4;
            }
            else {
                this->pc += 2;
            }
            break; 

        case 0x4000: //Opcode format: 4xkk. Skip next instruction if Vx != kk. Compare register Vx and kk. If not equal, increment PC by 4. 
            if (this->V[(this->opcode & 0x0F00) >> 8] != this->opcode & 0x00FF) {
                this->pc += 4;
            }
            else {
                this->pc += 2;
            }
            break; 

        case 0x5000: //Opcode format: 5xy0. Skip next instruction if Vx = Vy. Compare register Vx and register Vy. If equal, increment PC by 4.
            if (this->V[(this->opcode & 0x0F00) >> 8] == this->V[(this->opcode & 0x00F0) >> 4]){
                this->pc += 4;
            }
            else {
                this->pc += 2;
            }
            break; 
        case 0x6000: //Opcode format: 6xkk. Set Vx to kk. Put the value of kk into register Vx.
            this->V[(this->opcode & 0x0F00)>>8] = this->opcode & 0x00FF;
            this->pc += 2;
            break; 
        case 0x7000: //Opcode format: 7xkk. Set Vx = Vx + kk. Add value of kk to value in register Vx, then store result in Vx. 
            this->V[(this->opcode & 0x0F00) >> 8] += this->opcode & 0x0F00;
            this->pc += 2;
            break; 

        case 0x8000://Opcode format: 8xyU, where U can have values from 0 to 7, and E
            switch(this->opcode & 0x000F) {
                case 0x0000: //Opcode format: 8xy0. Set Vx = Vy. Store value of Vy in Vx.
                    this->V[(this->opcode & 0x0F00) >> 8] = this->V[(this->opcode & 0x00F0) >> 4];
                    this->pc += 2;
                    break;

                case 0x0001: //Opcode format: 8xy1. Set Vx = Vx OR Vy. Bitwise OR on Vx and Vy, then store result in Vx.
                    char vx = this->V[(this->opcode & 0x0F00) >> 8];
                    char vy = this->V[(this->opcode & 0x00F0) >> 4];
                    this->V[this->opcode & 0x0F00] = vx | vy;
                    this->pc += 2;
                    break;

                case 0x0002: //Opcode format: 8xy2. Set Vx = Vx AND Vy. Bitwise AND on Vx and Vy, then store result in Vx. 
                    char vx = this->V[(this->opcode & 0x0F00) >> 8];
                    char vy = this->V[(this->opcode & 0x00F0) >> 4];
                    this->V[this->opcode & 0x0F00] = vx & vy;
                    this->pc += 2;
                    break;

                case 0x0003: //Opcode format: 8xy3. Set Vx = Vx XOR Vy. Bitwise XOR on Vx and Vy, then store result in Vx.
                    char vx = this->V[(this->opcode & 0x0F00) >> 8];
                    char vy = this->V[(this->opcode & 0x00F0) >> 4];
                    this->V[this->opcode & 0x0F00] = vx ^ vy;
                    this->pc += 2;
                    break;

                case 0x0004: //Opcode format: 8xy4. Set Vx = Vx + Vy. Set Vf = carry. If sum of Vx and Vy, greater than 8 bits (255), set Vf to 1. Store 8 LSB of result in Vx.
                    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
                        V[0xF] = 1; // carry
                    }
                    else {
                        V[0xF] = 0;
                    }

                    this->V[(this->opcode & 0x0F00) >> 8] += V[(this->opcode & 0x00F0) >> 4];
                    this->pc += 2;
                    break;

                case 0x0005: //Opcode format: 8xy5. Set Vx = Vx - Vy. Set Vf to 1 if Vx > Vy. Store result in Vx. 
                    char vx = this->V[(this->opcode & 0x0F00) >> 8];
                    char vy = this->V[(this->opcode & 0x00F0) >> 4];
                    int sub = vx - vy;
                    if (Vx > Vy){
                        this->V[0xF] = 1;
                    }
                    else {
                        this->V[0xF] = 0;
                    }
                    this->V[(this->opcode & 0x0F00) >> 8] = sub & 0xFF; //ERROR SUSPECT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    this->pc += 2;
                    break;

                case 0x0006: //Opcode format: 8xy6. If LSB of Vx is 1, set Vf to 1. Divide Vx by 2.
                    if (this->V[(this->opcode & 0x0F00) >> 8] % 2 == 1){
                        this->V[0xF] = 1;
                    }
                    else {
                        this->V[0xF] = 0;
                    }
                    this->V[(this->opcode & 0x0F00) >> 8] = (this->V[(this->opcode & 0x0F00) >> 8]) / 2;
                    this->pc += 2;
                    break;

                case 0x0007: //Opcode format: 8xy7. Set Vx = Vy - Vx. Set Vf to 1 if Vy > Vx. Store result in Vx.
                    char vx = this->V[(this->opcode & 0x0F00) >> 8];
                    char vy = this->V[(this->opcode & 0x00F0) >> 4];
                    int sub = vy - vx;
                    if (Vx > Vy){
                        this->V[0xF] = 1;
                    }
                    else {
                        this->V[0xF] = 0;
                    }
                    this->V[(this->opcode & 0x0F00) >> 8] = sub & 0xFF; //ERROR SUSPECT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    this->pc += 2;
                    break;

                case 0x000E: //Opcode format: 8xyE. If MSB of Vx is 1, set Vf to 1. Multiply Vx by 2.
                    break;

                default: //Unknown command.
                    break;

            }

        case 0xA000: //set I to address NNN
            this->I = this->opcode & 0xFFF;
            break; 
        case 0xA000: //set I to address NNN
            this->I = this->opcode & 0xFFF;
            break; 
        case 0xA000: //set I to address NNN
            this->I = this->opcode & 0xFFF;
            break; 

        case 0xA000: //set I to address NNN
            this->I = this->opcode & 0xFFF;
            break; 

        case 0xA000: //set I to address NNN
            this->I = this->opcode & 0xFFF;
            break; 
        case 0xA000: //set I to address NNN
            this->I = this->opcode & 0xFFF;
            break; 
        case 0xA000: //set I to address NNN
            this->I = this->opcode & 0xFFF;
            break; 

        case 0xA000: //set I to address NNN
            this->I = this->opcode & 0xFFF;
            break; 
    }
    // Execute opcode

    // Update timers
}
