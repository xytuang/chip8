#include "chip8.h"

/*
0x000 - 0x1FF -> Chip8 interpreter

0x050 - 0xA00 -> Used for built in 4x5 pixel font set (0-F)

0x200 - 0xFFF -> Program ROM and work RAM

*/


unsigned short opcode; //35 opcodes for chip8, which are all 2 bytes big
unsigned char memory[4096]; //4k memory for chip8
unsigned char V[16]; //16 registers for chip8, last one is used as flag for carry bit

unsigned short I; //index register

unsigned short pc; //program counter has range from 0x000 to 0xFFF

unsigned char gfx[64 * 32]; //screen has 2048 pixels (64 * 32). Each pixel is either black or white (1 or 0)

//Two timer registers that count at 60 Hz. Will count down to zero if currently non-zero
unsigned char delayTimer;

unsigned char soundTime;

unsigned short stack[16]; //the stack(?)

unsigned short sp; //stack pointer

unsigned char key[16]; //HEX based keypad from 0x0-0xF. Use this variable to store current state of keypad. Each key has 16 bits :)



int main(int argc, char* argv[]) {
    //set up render system and register input callbacks
    setupGraphics();
    setupInput();

    //initialize chip8 system and load game into memory
    chip8.initialize();
    chip8.loadGame("pong");

    for(;;){
        chip8.emulateCycle();
        if(chip8.drawFlag()){
            drawGraphics();
        }
        chip8.setKeys(); //Store key press state (Press and release)
    }
    return 0;
}
