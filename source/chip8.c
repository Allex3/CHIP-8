#include "chip8.h"

#include <stdio.h>


/*
  Memory map:
  Total: 4096 bytes (4K)
  0x000 - 0x1FF     INTERPRETER
  0x050 - 0x0A0     Font set
  0x200 - 0xFFF     Program/Data space

  NOTE:
  opcodes are stored big-endian!
*/

/*
==========================================================
# CHIP-8 architecture
==========================================================
*/

unsigned short opcode;      // two byte opcode

//The Font set:
unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};
//a font letter explained, for me
/* 0 for example:
0xF0,  // 1111 0000  => ####
0x90,  // 1001 0000  => #  #
0x90,  // 1001 0000  => #  #
0x90,  // 1001 0000  => #  #
0xF0   // 1111 0000  => ####
*/

unsigned char memory[4096]; // 4096 1 byte Memory locations

unsigned char V[16];        // 16 1 byte registers Named Vx , x=0, 1.., D, E, F
// 16th register is for the carry flag, or other flags

unsigned short I;  // address/index register, used to store memory addresses (0x000 to 0xFFF)


unsigned short pc; // program counter, store the currecnly executing instruction's address(0x000 to 0xFFF)

unsigned char display[64 * 32]; // 2048 pixels screen, this array holds the pixel state, black or white(0 or 1)

unsigned char delayTimer; // two timers: They both count down at 60 hertz, until they reach 0.
unsigned char soundTImer; // delay - timing events of games; sound-sound effects

unsigned short stack[16]; // holds last address when a subroutine is called
unsigned short sp;        // stack pointer

unsigned char key[16]; // hex based Keypad (0x0-0xF)
