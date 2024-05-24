#ifndef CHIP8_H
#define CHIP8_H
#include <string>
#include <fstream>

class Chip8 {
    private:
        unsigned short opcode; //35 opcodes for chip8, which are all 2 bytes big
        unsigned char memory[4096]; //4k memory for chip8
        unsigned char V[16]; //16 registers for chip8, last one is used as flag for carry bit
        unsigned short I; //index register

        unsigned short pc; //program counter has range from 0x000 to 0xFFF

        unsigned char fontset[80];


        //Two timer registers that count at 60 Hz. Will count down to zero if currently non-zero
        unsigned char delayTimer;

        unsigned char soundTimer;

        unsigned short stack[16]; //the stack(?)

        unsigned short sp; //stack pointer

    public:
        bool drawFlag;

        unsigned char key[16]; //HEX based keypad from 0x0-0xF. Use this variable to store current state of keypad.
        unsigned char gfx[64 * 32]; //screen has 2048 pixels (64 * 32). Each pixel is either black or white (1 or 0)

        Chip8();
        ~Chip8();

        void loadGame(std::string filename);
        void initialize(); // Initialize registers and memory once

        void emulateCycle(); // Do opcode stuff and update timers
};

#endif
