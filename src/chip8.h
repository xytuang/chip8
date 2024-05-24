#ifndef CHIP8_H
#define CHIP8_H
#include <string>
#include <fstream>

class Chip8 {
    private:
        uint16_t stack[16];                 // Stack
        uint16_t sp;                        // Stack pointer

        uint8_t memory[4096];               // Memory (4k)
        uint8_t V[16];                      // V registers (V0-VF)

        uint16_t pc;                        // Program counter
        uint16_t opcode;                    // Current op code
        uint16_t I;                         // Index register

        uint8_t delayTimer;                // Delay timer
        uint8_t soundTimer;                // Sound timer
    public:
        bool drawFlag;

        uint8_t key[16]; //HEX based keypad from 0x0-0xF. Use this variable to store current state of keypad.
        uint8_t gfx[64 * 32]; //screen has 2048 pixels (64 * 32). Each pixel is either black or white (1 or 0)

        Chip8();
        ~Chip8();

        void loadGame(char* file_path);
        void initialize(); // Initialize registers and memory once

        void emulateCycle(); // Do opcode stuff and update timers
};

#endif
