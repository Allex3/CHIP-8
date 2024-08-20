#include "chip8.h"
#include "peripherals.h"

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

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

unsigned short opcode; // two byte opcode

// The Font set:
unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
// a font letter explained, for me
/* 0 for example:
0xF0,  // 1111 0000  => ####
0x90,  // 1001 0000  => #  #
0x90,  // 1001 0000  => #  #
0x90,  // 1001 0000  => #  #
0xF0   // 1111 0000  => ####
*/

unsigned char memory[4096]; // 4096 1 byte Memory locations

unsigned char V[16]; // 16 1 byte registers Named Vx , x=0, 1.., D, E, F
// 16th register is for the carry flag, or other flags

unsigned short I; // address/index register, used to store memory addresses (0x000 to 0xFFF)

unsigned short pc; // program counter, store the currecnly executing instruction's address(0x000 to 0xFFF)

unsigned char display[64 * 32]; // 2048 pixels screen, this array holds the pixel state, black or white(0 or 1)

unsigned char delayTimer; // two timers: They both count down at 60 hertz, until they reach 0.
unsigned char soundTimer; // delay - timing events of games; sound-sound effects
unsigned int soundFlag;

unsigned short stack[16]; // holds last address when a subroutine is called
unsigned short sp;        // stack pointer

unsigned char keypad[16]; // hex based Keypad (0x0-0xF)

/*
==========================================================
# CHIP-8 programming
==========================================================
*/

void initializeCHIP8(void)
{
    // Clear registers V0-VF
    for (int i = 0; i <= 4095; i++)
        memory[i] = 0;
    // Clear memory
    for (int i = 0; i <= 15; i++)
        V[i] = 0;
    // Load fontset
    memcpy(memory, fontset, sizeof(fontset));
    // first 512 positions are for the interpreter, so PC starts at position 0x200 (512)
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    // Initialize clear display
    initDisplay();
    // Clear stack
    for (int i = 0; i <= 15; i++)
        stack[i] = 0;

    // Reset timers
    delayTimer = 0;
    soundTimer = 0;
    soundFlag = 0;
}

// emulate a cycle = run a chip8 instruction, an opcode
/*
1. Fetch opcode - fetch the operation code at the address specified by the PC
  there's only one byte at each memory address, an opcode is two bytes long
  so we should fetch 2 consecutive bytes and merge them, that's the OPCODE
  Example:
        0xAC82 -> memory[pc] = 0xAC, memory[pc+1]=82,
        two hexadecimals are 8 bits
        opcode = memory[pc]<<8 | memory[pc+1]
            -> shift by 8 bytes left,
                to add 8 binary zeroes = two hexedimal zeroes so 0xAC00
            -> merge it by binary OR operation with memory[pc+1], so it is now 0xAC82
                thus the extra zeroes are filled in by the first 8 bits of 0x82

2. Decode the opcode
    - look up the optable to see what it means
    Example:
        at memory locatrion 0xAC82 we have ANNN
        this I to the address NNN (0xC82) (load I, NNN)

3. Execute the opcode
    - execute the aprsed op
    Example:
        0xAC82 -> store 0xC82 into the I register, as only 12 bits
        are storing the value we want to store, addresses have up to 12 bits
        So we can use bitwise AND to get rid of the last 4 bits
        which is the A in 0xAC82, and get only the last 12 bits, 0x0C82
        thus: 0xAC82 & 0x0FFF does this, it keeps only the set bits,
        and sets to 0 the last 4 bits
        --> I = opcode & 0x0FFF
            pc += 2 - go to next instruction, every instr. is two bytes long

4. Update timers
    - count to zero at 60Hz if they are not null
*/
void emulateCycle(void)
{
    opcode = memory[pc] << 8 | memory[pc + 1]; 

    //Update timers, decrement them if they are not null
    if (delayTimer > 0) delayTimer = -1;
    if (soundTimer > 0) {
        soundFlag = 1;
        puts("BEEP");
        soundFlag -= 1;
    }
}

// start filling the memory with instructions in the given ROM
// starting at position 512, reading data from the file in binary mode
void loadROM(char *filename)
{
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL) // file can't be opened
    {
        fprintf(stderr, "File to run is NULL.");
        return -1;
    }
    printf("Opening file %s\n", filename);

    // use fseek to get the cursor to the end of the file
    if (fseek(fp, 0, SEEK_END) < 0)
    {
        fprintf(stderr, "Error reading the file: fseek\n");
        return;
    }
    // get the file size in bytes
    size_t dataSize = ftell(fp);
    // how many bytes are there until the end of the file (that's what we used fseek for)
    // this is the number of bytes we read

    // Rewind the cursor back to the beginning of file
    rewind(fp);

    size_t bytesRead = fread(memory + 0x200, 1, dataSize, fp);
    // read the file contents (instructions) into memory

    fclose(fp);

    if (bytesRead != dataSize)
    {
        fprintf(stderr, "Can't read the full ROM file");
        return -2;
    }

    return 0;
}