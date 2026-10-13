#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"

const uint16_t ROM_START = 0x200;


uint16_t read_word(chip8 *sys, uint16_t address) {
    return ((uint16_t)(read_byte(sys, address) << 8) | read_byte(sys, address+1));
}

uint8_t read_byte(chip8 *sys, uint16_t address) {
    //printf("[][]  read_byte at address %#x: %#x\n", address, sys->memory[address]);
    return sys->memory[address];
}

void dump_memory(chip8 *sys, uint16_t start, uint16_t end) {
    printf("Memory from %u to %u\n", start, end);
    for (uint16_t x = start; x < end; x++) {
        printf("%8x\n", sys->memory[x]);
    }
    return;
}

void dump_reg(chip8 *sys, uint8_t reg) {
    printf("==> value from reg %u is %#x\n",reg, sys->reg[reg]);
}

void dump_registers(chip8 *sys, bool frames) {
    printf("\nREGISTERS:\n==========\n");
    
    // program counter:
    printf("PC:\t%#x\n", sys->pc);

    // I register
    printf("I:\t%#x\n", sys->I);

    // stack pointer
    printf("SP:\t\t%#x\n", sys->sp);
    if (frames) {
        for(int x=0;x<=15;x++) {
            printf("Stack Frame %x:\t\t%#x\n", x, sys->stack[x]);
        }
    }

    // gp registers
    for(int x = 0; x <=15;x++){
        printf("V%X:\t%#x\n", x, sys->reg[x]);
    }

    // delay
    printf("Delay:\t\t%#x\n", sys->delay);

    // sound
    printf("Sound:\t\t%#x\n", sys->sound);

    printf("===========\n\n\n");
}

void execute(chip8 *sys) {
    // get instructions from rom in memory
    uint16_t word = read_word(sys, sys->pc);
    printf("found word: %#x from address %u\n", word, sys->pc);

    // increment pc now that we have our data
    sys->pc+=2;
    // parse instruction pulled
    //printf("found instruction: %08X\n", (uint8_t)((instruction >> 4) & 0x00FF));
    //uint16_t nibble = (word & 0xFF00); 
    uint8_t high = (uint8_t)(word >> 8) & 0xFF;
    uint8_t low = (uint8_t)(word & 0xFF);
    //printf("high: %#x\n", high);
    //printf("low: %#x\n", low);

    uint8_t opcode = high & 0xF0;
    //printf("built opcode: %#x\n",opcode);

    // break nibble into categories
    switch(opcode) {
        case 0x00:
            // system routines
            //printf("found opcode 0x00!\n");
            if (word == 0x00EE) {
                printf("now returning from subroutine!\n");
                ret(sys);
            }
            if (word == 0x00e0) {
                printf("now clearing screen\n");
                cls(sys);
            }
            break;
        case 0x10: {
            //Jump Routines
            //printf("found opcode 0x10!\n");
            uint16_t addr = word & 0x0FFF;
            printf("Now jumping to address %#x\n", addr);
            jmp(sys, addr);
            break;
        }
        case 0x20: {
            // Call Routines
            //printf("found opcode 0x20!\n");
            uint16_t value = (word & 0x0FFF);
            printf("now calling routine at address %u/ %#x\n", value, value);
            call_nnn(sys, value);
            break;
        }
        case 0x30:
            // Skip Equial (SE vX kk; pc+=2)
            printf("found opcode 0x30!\n");
            break;
        case 0x40:
            // Skip Not Equial (SNE Vx kk; )
            printf("found opcode 0x40!\n");
            break;
        case 0x50:
            // Skip Equal (SE Vx Vy)
            printf("found opcode 0x50!\n");
            break;
        case 0x60: {
            //printf("found opcode 0x60!\n");
            // uint8_t reg = (nibble & 0x0F00);
            uint8_t reg = (high & 0x0F);
            printf("now loading %u to reg %d\n", low, reg);
            ld_kk(sys, reg, low);
            break;
        }
        case 0x70: {
            // ADD Reg (AD Vx kk)
            //printf("found opcode 0x70!\n");
            printf("now adding %u to value at reg %u\n", low, (high & 0x0F));
            add_reg_kk(sys, (high & 0x0F), low);
            break;
        }
        case 0x80:
            // Register Manipulation 
            printf("found opcode 0x80!\n");
            break;
        case 0x90:
            // SNE Vx Vy
            printf("found opcode 0x90!\n");
            break;
        case 0xA0: {
            // Load to I (LD nnn (12bits/3bytes))
            uint8_t bytes = (word & 0x0FFF);
            printf("now loading %#x to I\n", bytes);
            ld_i_nnn(sys, bytes);
            break;
        }
        case 0xB0:
            // Jump nnn + v0 (JUMP 0 nnn)
            printf("found opcode 0xB0!\n");
            break;
        case 0xC0:
            // Generate Rand (RAND vX kk)
            printf("found opcode 0xC0!\n");
            break;
        case 0xD0: {
            // Draw vX vY n ()
            //printf("found opcode 0xD0!\n");
            uint8_t reg_x = (word & 0x0F00);
            uint8_t reg_y = (word & 0x00F0);
            uint8_t bytes = (word & 0x000F);
            printf("now drawing at [%u,%u], pulling %#x bytes starting from addr %#x\n", reg_x, reg_y, bytes, sys->I); 
            draw(sys, reg_x, reg_y, bytes);
            break;
        }
        case 0xE0:
            // Skip instructions
            printf("found opcode 0xE0!\n");
            break;
        case 0xF0:
            // Various sound, delay, and input instrucitons
            printf("found opcode 0xF0!\n");
            break;
        default:
            //printf("couldn't find opcode\n");
            printf("opcode: %#x\n",opcode);
            break;
    }
    
    // execute opcode
    if (word == 0) {
        exit(0);
    }

    // increment pc forward to next instruction
    // sys->pc+=2; //2 byte instructions
}

int load_rom(chip8 *sys, const char *filename) {

    // first get file handle and open file to read
    FILE *file = fopen(filename, "rb");
    if (file == NULL ) {
        printf("unable to open filename: %s\n", filename);
        // return 0 for unable to open filename. 
        return 0;
    }
    
    // define buffer to hold data, then read file.
    // this is an issue if files are larger. 
    uint8_t data[3584];
    size_t dataRead = fread(data, sizeof(uint8_t), 3584, file);

    // copy data into chip memory.
    // for each 8 bit chunk, add to sys->memory[] and increment
    for (size_t x = 0; x < dataRead; x++) {
        sys->memory[ 0x200 + x ] = data[x];
    }
    // set the program counter
    sys->pc = 0x200;
    
    // if no errors, return 1
    return 0;
}

void load_sprites(chip8 *sys) {
    uint8_t font_data[80] = {
        // zero
        0b11110000,
        0b10010000,
        0b10010000,
        0b10010000,
        0b11110000, 
        // one
        0b00100000,
        0b01100000,
        0b00100000,
        0b00100000,
        0b01110000,
        // two
        0b11110000,
        0b00010000,
        0b11110000,
        0b10000000,
        0b11110000,
        // three
        0b11110000,
        0b00010000,
        0b01110000,
        0b00010000,
        0b11110000,
        // four
        0b10010000,
        0b10010000,
        0b11110000,
        0b00010000,
        0b00010000,
        // five
        0b11110000,
        0b10000000,
        0b11110000,
        0b00010000,
        0b11110000,
        // six
        0b11110000,
        0b10000000,
        0b11110000,
        0b10010000,
        0b11110000,
        // seven
        0b11110000,
        0b00010000,
        0b00100000,
        0b01000000,
        0b01000000,
        // eight
        0b11110000,
        0b10010000,
        0b11110000,
        0b10010000,
        0b11110000,
        // nine
        0b11110000,
        0b10010000,
        0b11110000,
        0b00010000,
        0b00010000,
        // A
        0b11110000,
        0b10010000,
        0b11110000,
        0b10010000,
        0b10010000,
        // B
        0b11100000,
        0b10010000,
        0b11100000,
        0b10010000,
        0b11100000,
        // C
        0b11110000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b11110000,
        // D
        0b11100000,
        0b10010000,
        0b10010000,
        0b10010000,
        0b11100000,
        // E
        0b11110000,
        0b10000000,
        0b11110000,
        0b10000000,
        0b11110000,
        // F
        0b11110000,
        0b10000000,
        0b11110000,
        0b10000000,
        0b10000000,
    };
    memcpy(sys->memory, font_data, sizeof(font_data));
    printf("now done with load_sprites");
}

void debug_sprites(chip8 *sys) {
    printf("Printing sprite memory!\n=======================\n");
    for (uint8_t i = 0; 0 < 80; i++) {
        printf("Address %#x: %#x\n",i, sys->memory[i]);
    }
    return;
}