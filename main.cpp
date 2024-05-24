#include "chip8.h"

int main(int argc, char* argv[]) {
    Chip8 chip8 = Chip8();
    //set up render system and register input callbacks
    //setupGraphics();
    //setupInput();

    //initialize chip8 system and load game into memory
    chip8.initialize();
    chip8.loadGame("pong");

    for(;;){
        chip8.emulateCycle();
        if(chip8.drawFlag){
            drawGraphics();
        }
        chip8.setKeys(); //Store key press state (Press and release)
    }
    return 0;
}
