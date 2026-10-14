#include "chip8.h"
#include "cpu.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void ld_kk(chip8 *sys, uint8_t reg, uint8_t value) {
    // load 8 byte kk to register
    sys->reg[reg] = value;
    //dump_reg(sys, reg);
    return;
}

void add_reg_kk(chip8 *sys, uint8_t reg, uint8_t value) {
    sys->reg[reg] += value;
    //dump_reg(sys,reg);
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

void se_vX_kk(chip8 *sys, uint8_t reg, uint8_t value) {
    // Skip next instruction if vX == kk
    if (sys->reg[reg] == value) {
        sys->pc+=2;
    }
}

void sne_vX_kk(chip8 *sys, uint8_t reg, uint8_t bytes) {
    // Skip next instruction if Vx != kk.
    if (sys->reg[reg] != bytes) {
        sys->pc+=2;
    }
}

void se_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y) {
    if (sys->reg[reg_x] == sys->reg[reg_y]) {
        sys->pc+=2;
    }
}

void ld_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y) {
    sys->reg[reg_x] = sys->reg[reg_y];
}

void or_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y) {
    sys->reg[reg_x] |= sys->reg[reg_y];
}

void and_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y) {
    sys->reg[reg_x] &= sys->reg[reg_y];
}

void xor_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y) {
    sys->reg[reg_x] ^= sys->reg[reg_y];
}

void add_vX_vY(chip8 *sys, uint16_t reg_x, uint16_t reg_y) {
    // if reg[x] and reg[y] are larger than 255, vF is 
    // set 1. Only lowest 8 bits are stored back to vX
    // x = 75, y = 200
    // 220, 198
    uint16_t result = (sys->reg[reg_x] + sys->reg[reg_y]);
    if (result > 255) {
        sys->reg[0xF] = 1;
    }
    sys->reg[reg_x] = (uint8_t)result;
    // if ((255 - sys->reg[reg_x]) > reg_y) {
    //     sys->reg[0xF] = 1;
    // }

}

void sub_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y) {
    // Set Vx = Vx - Vy, set VF = NOT borrow.
    sys->reg[0xF] = (sys->reg[reg_x] > sys->reg[reg_y]) ? 1 : 0;
    sys->reg[reg_x] -= sys->reg[reg_y];
}

void shr_vX(chip8 *sys, uint8_t reg_x) {
    // If the least-significant bit of Vx is 1, then VF 
    // is set to 1, otherwise 0. Then Vx is divided by 2.
    sys->reg[0xF] = (sys->reg[reg_x] & 0x01) ? 1 : 0;
    sys->reg[reg_x] = sys->reg[reg_x]/2;
}

void subn_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y) {
    // set vX = vY - vX , 
    sys->reg[0xF] = (sys->reg[reg_y] > sys->reg[reg_x]) ? 1 : 0;
    sys->reg[reg_x] = sys->reg[reg_y] - sys->reg[reg_x];
}

void shl_vX(chip8 *sys, uint8_t reg_x) {
    sys->reg[0xF] = (sys->reg[reg_x] & 0x80) ? 1 : 0;
    sys->reg[reg_x] = sys->reg[reg_x]*2;
}

void sne_vX_vY(chip8 *sys, uint8_t reg_x, uint8_t reg_y) {
    // Skip next instruction if Vx != Vy.
    if (sys->reg[reg_x] != sys->reg[reg_y]) {
        sys->pc+=2;
    }
}

void jump_v0(chip8 *sys, uint16_t bytes) {
    sys->pc = bytes + sys->reg[0];
}

void rand_vX_kk(chip8 *sys, uint8_t reg, uint8_t bytes) {
    //  Set Vx = random byte AND kk.
    // Seed the random number generator once at startup
    srand((unsigned int)time(NULL));

    // Generate a random value between 0 and 255
    uint8_t random = rand() % 256;
    sys->reg[reg] = (random & bytes);
}

void skp_vX( chip8 *sys, uint8_t reg) {
    //SKP vX
    //  Skip next instruction if key with the value 
    //  of Vx is pressed.
    if ( sys->keypad[reg] ) {
        sys->pc += 2;
    }
}

void skpn_vX( chip8 *sys, uint8_t reg) {
    //SKP vX
    //  Skip next instruction if key with the value 
    //  of Vx is pressed.
    if ( !sys->keypad[reg] ) {
        sys->pc += 2;
    }
}

void ld_vX_delay(chip8 *sys, uint8_t reg) {
    sys->reg[reg] = sys->delay;
}

void ld_delay_vX(chip8 *sys, uint8_t reg) {
    sys->delay = sys->reg[reg];
}

void ld_vX_key(chip8 *sys, uint8_t reg) {
    sys->wait_buff = sys->reg[reg];
    sys->state = WAITING;
}

void load_delay_vX(chip8 *sys, uint8_t reg) {
    sys->delay = sys->reg[reg];
}

void ld_sound_vX(chip8 *sys, uint8_t reg) {
    sys->sound = sys->reg[reg];
}

void add_I_vX(chip8 *sys, uint8_t reg) {
    sys->I += sys->reg[reg];
}

void ld_f_vX(chip8 *sys, uint8_t reg) {
    sys->I = sys->memory[reg * 5];
}

void ld_b_vX(chip8 *sys,uint8_t reg) {
    sys->memory[sys->I] = (sys->memory[reg] / 100) % 10;
    sys->memory[sys->I+1] = (sys->memory[reg] / 10) % 10;
    sys->memory[sys->I+2] = sys->memory[reg] % 10;

}

uint8_t sdl_to_ch8_key(SDL_Keycode key) {
    switch(key) {
        case SDLK_0: {return 0x0;}
        case SDLK_1: {return 0x1;}
        case SDLK_2: {return 0x2;}
        case SDLK_3: {return 0x3;}
        case SDLK_4: {return 0x4;}
        case SDLK_5: {return 0x5;}
        case SDLK_6: {return 0x6;}
        case SDLK_7: {return 0x7;}
        case SDLK_8: {return 0x8;}
        case SDLK_9: {return 0x9;}
        case SDLK_a: {return 0xA;}
        case SDLK_b: {return 0xB;}
        case SDLK_c: {return 0xC;}
        case SDLK_d: {return 0xD;}
        case SDLK_e: {return 0xE;}
        case SDLK_f: {return 0xF;}
        default: return 0;
    }
}