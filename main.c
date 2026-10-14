#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"
#include "cpu.h"
#include "display.h"


int main(int argc, char *argv[]) {
    
    if (argc < 1) {
        printf("Usage: ch8mu <filename>\n");
        exit(-1);
    }
    
    // build emulator
    chip8 system = {0};
    system.state = RUNNING;

    // init sprite data
    load_sprites(&system);
    //debug_sprites(&system);

    if (!init_display(&system)) {
        return 1; // Exit if SDL fails
    }

    char* fName = argv[1];
    // load rom data into emulator memory.
    //load_rom(&system, "test.ch8");
    if(load_rom(&system, fName)) {
        cleanup_display(&system);
        exit(-1);
    }
    

    //printf("ROM loaded! Dumping registers!\n");
    // dump_registers(&system, false);
    
    // begin execution
    printf("Now executing ROM data!\n");
    SDL_Event event;
    while (true) {
        // test pixels
        // system.display[1][1] = 1; // Top left corner
        // system.display[62][30] = 1; // Bottom right corner
        // system.display[32][16] = 1; // Half point
        if (system.state == RUNNING) {
            execute(&system);
        } else {
            for (int i = 0; i < 16; i++) {
                if (system.keypad[i]) {
                    // A key was pressed! Store it in the target register
                    system.reg[system.wait_buff] = i;
                    // Set state back to running so we can execute instructions again
                    system.state = RUNNING;
                    // Important: The PC is already pointing at the NEXT instruction
                    // because our execute() function increments it.
                    break;
                }
            }
        }
        render_display(&system);
        //dump_registers(&system, 0);
        //getchar();
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                cleanup_display(&system);
                return 0;
            }
        
            if (event.type == SDL_KEYDOWN) {
                int key_id = sdl_to_ch8_key(event.key.keysym.sym);
                if ( key_id == -1 ) { 
                    system.keypad[key_id] = 1;
                }
            }
            if (event.type == SDL_KEYUP) {
                int key_id = sdl_to_ch8_key(event.key.keysym.sym);
                if ( key_id != -1) {
                    system.keypad[key_id] = 0;
                }
            }
        }
    }

    cleanup_display(&system);
    return 0;
    
}
