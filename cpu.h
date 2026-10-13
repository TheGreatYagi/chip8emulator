#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdio.h>

#include "chip8.h"


void execute(chip8 *sys);
int load_rom(chip8 *sys, const char *filename);
uint16_t read_word(chip8 *sys, uint16_t address);
uint8_t read_byte(chip8 *sys, uint16_t address);
void dump_registers(chip8 *sys, bool frames);
void dump_reg(chip8 *sys, uint8_t reg);
void dump_memory(chip8 *sys, uint16_t start, uint16_t end);
void load_sprites(chip8 *sys);
void debug_sprites(chip8 *sys);


#endif 