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


}

void chip8::emulateCycle(){
    // Fetch opcode
    this->opcode = memory[pc] << 8 | memory[pc+1];
    // Decode opcode -- 35 opcodes :)))))))
    switch(this->opcode & 0xF000) {
        
    }
    // Execute opcode

    // Update timers
}
