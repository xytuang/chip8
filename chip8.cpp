#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstdlib>

Chip8::Chip8(){};

Chip8::~Chip8(){};

void Chip8::loadGame(std::string filename) {
    char file_path[filename.size()];
    for (int i = 0; i < filename.size(); i++) {
        file_path[i] = filename[i];
    }
    // Open ROM file
    FILE* rom = fopen(file_path, "rb");
    if (rom == NULL) {
        std::cerr << "Failed to open ROM" << std::endl;
        return false;
    }

    // Get file size
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    // Allocate memory to store rom
    char* rom_buffer = (char*) malloc(sizeof(char) * rom_size);
    if (rom_buffer == NULL) {
        std::cerr << "Failed to allocate memory for ROM" << std::endl;
        return false;
    }

    // Copy ROM into buffer
    size_t result = fread(rom_buffer, sizeof(char), (size_t)rom_size, rom);
    if (result != rom_size) {
        std::cerr << "Failed to read ROM" << std::endl;
        return false;
    }

    // Copy buffer to memory
    if ((4096-512) > rom_size){
        for (int i = 0; i < rom_size; ++i) {
            this->memory[i + 512] = (uint8_t)rom_buffer[i];   // Load into memory starting
                                                        // at 0x200 (=512)
        }
    }
    else {
        std::cerr << "ROM too large to fit in memory" << std::endl;
        return false;
    }

    // Clean up
    fclose(rom);
    free(rom_buffer);

    return true;
}

void Chip8::initialize(){
    // Initialize registers and memory once
    this->pc == 0x200;
    this->opcode = 0;
    this->I = 0;
    this->sp = 0;

    //clear display
    for (int i = 0; i < 2048; i++) {
        this->gfx[i] = 0;
    }

    //clear stack, registers, keys
    for (int i = 0; i < 16; i++) {
        this->stack[i] = 0;
        this->V[i] = 0;
        this->key[i] = 0;
    }

    //clear memory
    for (int i = 0; i < 4096; i++) {
        this->memory[i] = 0;
    }
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
    srand(time(NULL));
}

void chip8::emulateCycle(){
    // Fetch opcode
    this->opcode = memory[pc] << 8 | memory[pc+1];
    // Decode opcode -- 35 opcodes :)))))))
    switch(this->opcode & 0xF000) {

        case 0x0000:
            switch(this->opcode & 0x00FF) {
                case 0x00E0: //clear screen
                    for (int i = 0; i < 2048; i++){
                        this->gfx[i] = 0;
                    }
                    this->drawFlag = true;
                    this->pc += 2;
                    break;
                case 0x00EE: //Return from subroutine. Set PC to value at top of stack. Decrement SP.
                    this->sp--;
                    this->pc = this->stack[sp];
                    pc += 2;
                    break;
                default: //unknown command
                    exit(1);
            }
        case 0x1000: //Jump to location NNN. Set PC to NNN.
            this->pc = this->opcode & 0x0FFF;
            break; 

        case 0x2000: //Call subroutine at NNN. Increment SP, store current PC at top of stack. Set PC to NNN.
            this->sp++;
            this->stack[sp-1] = this->pc;
            this->pc = this->opcode & 0x0FFF;
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
            this->V[(this->opcode & 0x0F00) >> 8] += this->opcode & 0x00FF;
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
                    if (Vx > Vy){
                        this->V[0xF] = 1;
                    }
                    else {
                        this->V[0xF] = 0;
                    }
                    this->V[(this->opcode & 0x0F00) >> 8] = vx - vy;
                    this->pc += 2;
                    break;

                case 0x0006: //Opcode format: 8xy6. If LSB of Vx is 1, set Vf to 1. Divide Vx by 2.
                    if (this->V[(this->opcode & 0x0F00) >> 8] & 0x01 == 1){
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
                    if (vy > vx){
                        this->V[0xF] = 1;
                    }
                    else {
                        this->V[0xF] = 0;
                    }
                    this->V[(this->opcode & 0x0F00) >> 8] = vy - vx;
                    this->pc += 2;
                    break;

                case 0x000E: //Opcode format: 8xyE. If MSB of Vx is 1, set Vf to 1. Multiply Vx by 2.
                    if (this->V[(this->opcode & 0x0F00) >> 8] & 0x80 == 1){
                        this->V[0xF] = 1;
                    }
                    else {
                        this->V[0xF] = 0;
                    }
                    this->V[(this->opcode & 0x0F00) >> 8] = (this->V[(this->opcode & 0x0F00) >> 8]) * 2;
                    this->pc += 2;
                    break;

                default: //Unknown command.
                    exit(1);

            }

        case 0x9000: //Opcode format: 9xy0. Skip next instruction if Vx != Vy. Compare values in Vx and Vy. If not equal, increment PC by 4
            if (this->V[(this->opcode & 0x0F00) >> 8] != this->V[(this->opcode & 0x00F0) >> 4]){
                this->pc += 4;
            }
            else {
                this->pc += 2;
            }
            break;

        case 0xA000: //Opcode format: ANNN. Set I to address NNN. Value of register I is set to NNN.
            this->I = this->opcode & 0x0FFF;
            this->pc += 2;
            break; 

        case 0xB000: //Opcode format: BNNN. Jump to location V0 + NNN. Set PC to value in V0 + NNN.
            this->pc = this->V[0] + (this->opcode & 0x0FFF);
            break; 

        case 0xC000: //Opcode format: Cxkk. Set Vx = random byte AND kk. Generate random number from 0 to 255, and it with kk. Store result in Vx.
            unsigned char randomByte = rand() % 256;
            this->V[(this->opcode & 0x0F00) >> 8] = randomByte & (this->opcode & 0x00FF);
            this->pc += 2;
            break; 

        case 0xD000: //Opcode format: Dxyn. Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
            unsigned char vx = this->V[(this->opcode & 0x0F00) >> 8] % 64;

            unsigned char vy = this->V[(this->opcode & 0x00F0) >> 4] % 32;

            this->V[0xF] = 0;

            for (int i = 0; i < i < (this->opcode & 0x000F); i++) {
                unsigned char pixel = this->memory[this->I + i];
                for (int j = 0; j < 8; j++) {
                    if (pixel & (0x80 >> j) != 0) {
                        if (this->gfx[(vx + j + ((vy + i) * 64))] == 1) {
                            this->V[0xF] = 1;
                        }
                        this->gfx[(vx + j + ((vy + i) * 64))] ^= 1;
                    }
                }
            }
            this->drawFlag = true;
            this->pc += 2;
            break; 

        case 0xE000: //Opcode format: ExVW, where VW is either 9E of A1
            switch (this->opcode & 0x00FF) {
                case 0x009E: //Opcode format: Ex9E. Skip next instruction if key with value of Vx is pressed. If key corresponding to value in Vx is pressed, increment PC by 4
                    if(this->key[this->V[(this->opcode & 0x0F00) >> 8]] == 1) {
                        this->pc += 4;
                    }
                    else {
                        this->pc += 2;
                    }
                    break;

                case 0x00A1: //Opcode format: ExA1. Skip next instruction if key with value of Vx is not pressed. If key corresponding to value in Vx is not pressed, increment PC by 4
                    if(this->key[this->V[(this->opcode & 0x0F00) >> 8]] == 0) {
                        this->pc += 4;
                    }
                    else {
                        this->pc += 2;
                    }
                    break;
                default: //Unknown command
                    exit(1);
            }

        case 0xF000: //Opcode format: FxVW, where VW is in 07, 0A, 15, 18, 1E,29, 33, 55, 65
            switch (this->opcode & 0x00FF) {
                case 0x0007: //Opcode format: Fx07. Set Vx = delay timer value. 
                    this->V[(this->opcode & 0x0F00) >> 8] = this->delayTimer;
                    pc += 2;
                    break;

                case 0x000A: //Opcode format: Fx0A. Wait for key press. Store value of key in Vx.
                    bool keyFound = false;
                    for (unsigned int i = 0; i < 16; i++) {
                        if (this->key[i]) {
                            this->V[(this->opcode & 0x0F00) >> 8] = i; 
                            keyFound = true;
                            break;
                        }
                    }
                    if (!keyFound) {
                        this->pc -= 2;
                    }
                    break;

                case 0x009E: //Opcode format: Ex9E. Skip next instruction if key with value of Vx is pressed. If key corresponding to value in Vx is pressed, increment PC by 4
                    if(this->key[this->V[(this->opcode & 0x0F00) >> 8]] == 1) {
                        this->pc += 4;
                    }
                    else {
                        this->pc += 2;
                    }
                    break;

                case 0x0015: //Opcode format: Fx15. Set delay timer = value in Vx. 
                    this->delayTimer = this->V[(this->opcode & 0x0F00) >> 8];
                    this->pc += 2;
                    break;

                case 0x0018: //Opcode format: Fx18. Set sound timer = value in Vx. 
                    this->soundTimer = this->V[(this->opcode & 0x0F00) >> 8];
                    this->pc += 2;
                    break;


                case 0x001E: //Opcode format: Fx1E. Set I = I + Vx. 
                    this->I += this->V[(this->opcode & 0x0F00) >> 8];
                    this->pc += 2;
                    break;

                case 0x0029: //Opcode format: Fx29. Set I = location of sprite for digit Vx.
                    char vx = this->V[(this->opcode & 0x0F00) >> 8];
                    int spritePosition = (vx % 16) * 5;
                    this->I = &(this->memory[spritePosition]);
                    this->pc += 2;
                    break;

                case 0x0033: //Opcode format: Fx33. Store BCD representation of Vx in memory locations I,I+1,I+2. 
                    this->memory[this->I]     = this->V[(this->opcode & 0x0F00) >> 8] / 100;
                    this->memory[this->I + 1] = (this->V[(this->opcode & 0x0F00) >> 8] / 10) % 10;
                    this->memory[this->I + 2] = (this->V[(this->opcode & 0x0F00) >> 8] % 100) % 10;
                    this->pc += 2;
                    break;

                case 0x0055: //Opcode format: Fx55. Store registers v0 through vx in memory starting at location I.
                    for (unsigned int i = 0; i <= ((this->opcode & 0x0F00) >> 8); i++) {
                        this->memory[this->I + i] = this->V[i];
                    }
                    this->pc += 2;
                    break;

                case 0x0065: //Opcode format: Fx65. Read registers v0 through vx from memory starting at location I..
                    for (unsigned int i = 0; i <= ((this->opcode & 0x0F00) >> 8); i++) {
                        this->V[i] = this->memory[this->I + i];
                    }
                    this->pc += 2;
                    break;
                
                default: //Unknown command
                    exit(1);
            }
    }
    // Update timers
    if (this->soundTimer > 0) {
        --soundTimer;
    }
    if (this->delayTimer > 0) {
        --delayTimer;
    }
}
