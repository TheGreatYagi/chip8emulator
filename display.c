#include "display.h"

#define SCALE 10

bool init_display(chip8 *sys) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't init SDL, SDL Error: %s.\n", SDL_GetError());
        return false;
    }

    sys->window = SDL_CreateWindow(
        "Ch8mu",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        64 * SCALE,
        32 * SCALE,
        SDL_WINDOW_SHOWN
    );

    if(!sys->window) {
        printf("Couldn't create window, SDL error: %s\n", SDL_GetError());
        return false;
    }

    sys->renderer = SDL_CreateRenderer(sys->window, -1, SDL_RENDERER_ACCELERATED);
    if (!sys->renderer) {
        printf("Couldn't create renderer, SDL error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

void render_display(chip8 *sys) {
    // Clear the screen to black
    SDL_SetRenderDrawColor(sys->renderer, 0, 0, 0, 255);
    SDL_RenderClear(sys->renderer);

    // Set draw color to white for the pixels
    SDL_SetRenderDrawColor(sys->renderer, 255, 255, 255, 255);

    // Loop through the display array
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++) {
            if (sys->display[x][y] == 1) {
                // Define a rectangle for the scaled pixel
                SDL_Rect rect = { x * SCALE, y * SCALE, SCALE, SCALE };
                SDL_RenderFillRect(sys->renderer, &rect);
            }
        }
    }

    // 4. Push everything to the screen
    SDL_RenderPresent(sys->renderer);
}

void cleanup_display(chip8 *sys) {
    SDL_DestroyRenderer(sys->renderer);
    SDL_DestroyWindow(sys->window);
    SDL_Quit();
}