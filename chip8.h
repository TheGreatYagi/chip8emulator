#ifndef CHIP8_H
#define CHIP8_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

typedef enum {
    WAITING,
    RUNNING
} cpu_state;

typedef struct {
    uint16_t stack[16]; // stack frames
    uint16_t pc; // program counter
    uint16_t I; // I register
    uint8_t memory[0x1000]; //4KB memory
    uint8_t display[64][32];
    uint8_t keypad[16];
    uint8_t reg[16]; // V0 - VF
    uint8_t delay;
    uint8_t sound;
    uint8_t sp; // stack pointer
    uint8_t wait_buff; // buffer for waiting
    SDL_Window *window; // add sdl stuffs here for easier management
    SDL_Renderer *renderer;

    cpu_state state;

} chip8;

void ld_kk(chip8 *sys, uint8_t reg, uint8_t value);
void add_reg_kk(chip8 *sys, uint8_t reg, uint8_t value);
void call_nnn(chip8 *sys, uint16_t value);
void ret(chip8 *sys);
void jmp(chip8 *sys, uint16_t address);
void cls(chip8 *sys);
void draw(chip8 *sys, uint8_t reg_x, uint8_t reg_y, uint8_t bytes);
void x_or(chip8 *sys, uint8_t reg_x, uint8_t reg_y);
void ld_i_nnn(chip8 *sys, uint16_t bytes);
void se_vX_kk(chip8 *sys, uint8_t reg, uint8_t bytes);
void sne_vX_kk(chip8 *sys, uint8_t reg, uint8_t bytes);
void se_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y);
void ld_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y);
void or_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y);
void and_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y);
void xor_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y);
void add_vX_vY(chip8 *sys, uint16_t reg_x, uint16_t reg_y);
void sub_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y);
void shr_vX(chip8 *sys, uint8_t reg_x);
void subn_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y);
void shl_vX(chip8 *sys, uint8_t reg_x);
void sne_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y);
void jump_v0(chip8 *sys, uint16_t bytes);
void rand_vX_kk(chip8 *sys, uint8_t reg, uint8_t bytes);
void skp_vX(chip8 *sys, uint8_t reg);
void skpn_vX(chip8 *sys, uint8_t reg);
void ld_vX_delay(chip8 *sys, uint8_t reg);
void ld_vX_key(chip8 *sys, uint8_t reg);
void ld_delay_vX(chip8 *sys, uint8_t reg);
void ld_sound_vX(chip8 *sys, uint8_t reg);
void add_I_vX(chip8 *sys, uint8_t reg);
void ld_f_vX(chip8 *sys, uint8_t reg);
void ld_b_vX(chip8 *sys,uint8_t reg);

uint8_t sdl_to_ch8_key(SDL_Keycode key);


#endif