#include "chip8.h"
#include <chrono>
#include <thread>
#include <SDL.h>

using namespace std;

uint8_t keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

int main(int argc, char* argv[]) {
    Chip8 chip8 = Chip8();
    
    //set up render system and register input callbacks
    int w = 1024;                   // Window width
    int h = 512;                    // Window height

    // The window we'll be rendering to
    SDL_Window* window = NULL;

    // Initialize SDL
    if ( SDL_Init(SDL_INIT_EVERYTHING) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        exit(1);
    }
    // Create window
    window = SDL_CreateWindow(
            "CHIP-8 Emulator",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            w, h, SDL_WINDOW_SHOWN
    );
    if (window == NULL){
        printf( "Window could not be created! SDL_Error: %s\n",
                SDL_GetError() );
        exit(2);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, w, h);

    // Create texture that stores frame buffer
    SDL_Texture* sdlTexture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            64, 32);

    // Temporary pixel buffer
    uint32_t pixels[2048];


    load:
    chip8.loadGame(argv[1]);

    for(;;){
        chip8.emulateCycle();
        
        // Process SDL events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) exit(0);

            // Process keydown events
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    exit(0);

                if (e.key.keysym.sym == SDLK_F1)
                    goto load;      // *gasp*, a goto statement!
                                    // Used to reset/reload ROM

                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.key[i] = 1;
                    }
                }
            }
            // Process keyup events
            if (e.type == SDL_KEYUP) {
                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.key[i] = 0;
                    }
                }
            }
        }

        if(chip8.drawFlag){
            chip8.drawFlag = false;
            // Store pixels in temporary buffer
            for (int i = 0; i < 2048; ++i) {
                uint8_t pixel = chip8.gfx[i];
                pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
            }
            // Update SDL texture
            SDL_UpdateTexture(sdlTexture, NULL, pixels, 64 * sizeof(Uint32));
            // Clear screen and render
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1200));
    }
    return 0;
}
