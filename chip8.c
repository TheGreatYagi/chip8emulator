#include "chip8.h"
#include "cpu.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void ld_kk(chip8 *sys, uint8_t reg, uint8_t value) {
    // load 8 byte kk to register
    sys->reg[reg] = value;
    dump_reg(sys, reg);
    return;
}

void add_reg_kk(chip8 *sys, uint8_t reg, uint8_t value) {
    sys->reg[reg] += value;
    dump_reg(sys,reg);
    return;
}

void call_nnn(chip8 *sys, uint16_t value) {
    // push current pc to stack
    printf("now putting value %#x into stack[%u]\n",sys->pc, sys->sp);
    sys->stack[sys->sp] = sys->pc;

    // increment stack pointer
    sys->sp++;

    // set pc to nnn

    // value has offset, need to accomodate
    //value = (value - 0x200);
    printf("call_nnn got value: %u / %#x\n", value, value);
    sys->pc = value;
    dump_registers(sys,true);
    dump_memory(sys, value, value+2);

}

void ret(chip8 *sys) {
    // pop value at stack pointer and assign to pc
    printf("pc before: %#x\n", sys->pc);
    printf("sp is: %u\n",sys->sp);
    sys->pc = sys->stack[sys->sp-1];
    printf("pc after: %#x\n", sys->pc);
    // decrement stack pointer
    sys->sp--;
}

void jmp(chip8 *sys, uint16_t address) {
    sys->pc = address;
}

void cls(chip8 *sys) {
    for(int x = 0; x< 64; x++){
        for( int y = 0; y<32; y++){
            sys->display[x][y] = 0;
        }
    }
}

void draw(chip8 *sys, uint8_t reg_x, uint8_t reg_y, uint8_t bytes) {
    // Display n-byte sprite starting at
    //  memory location I at (Vx, Vy), set VF = collision.
    
    // The interpreter reads n bytes from memory, starting
    //  at the address stored in I. These bytes are 
    // then displayed as sprites on screen at coordinates
    //  (Vx, Vy). 
    // 
    // Sprites are XORed onto the existing screen. If this
    // causes any pixels to be erased, VF is set to 1, 
    // otherwise it is set to 0. If the sprite is 
    // positioned so part of it is outside the 
    // coordinates of the display, it wraps around to the 
    // opposite side of the screen. See instruction 8xy3 
    // for more information on XOR, and section 2.4, 
    // Display, for more information on the Chip-8 screen
    //  and sprites.

   
    // pull x and y coords
    uint8_t start_x = sys->reg[reg_x];
    uint8_t start_y = sys->reg[reg_y];

    // set reg vF to 0
    sys->reg[0xF] = 0;

    // calculate x y wrap around
    //uint8_t x_wrap = ( (start_x + 8) > 64 ) ? ( start_x + 8 ) % 64 : 0; // only need to worry as we write left to right
    //uint8_t y_wrap = ( (start_y + bytes) > 32 ) ? (bytes + start_y) % 32 : 0; // only need to worry about top as we write up


    // read n bytes starting from sys->I
    //uint8_t to_draw[bytes];
    for (uint8_t x = 0; x < bytes; x++){
        uint8_t sprite_line = sys->memory[sys->I+x]; // line to draw
        uint8_t mask = 0x80;
        for (uint8_t bit = 0; bit < 8; bit++) {
            uint8_t final_x = (start_x + bit) % 64; // auto handle wrap around
            uint8_t final_y = (start_y + x) % 32;

            if (sprite_line & mask) { // found byte to draw

                if (sys->display[final_x][final_y]) { // colision check
                    sys->reg[0xF] = 1;
                }
                // xor the bit in the display for drawing
                sys->display[final_x][final_y] ^= 1;
            }
            mask >>= 1; // shift mask by one bit right
        }

    }

}

void ld_i_nnn(chip8 *sys, uint16_t bytes) {
    sys->I = bytes;
}

void x_or(chip8 *sys, uint8_t reg_x, uint8_t reg_y) {
    sys->reg[reg_x] ^= reg_y;
}