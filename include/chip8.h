#ifndef CHIP8_H_
#define CHIP8_H_

extern unsigned short opcode;
extern unsigned char fontset[80];
extern unsigned char memory[4096];
extern unsigned char V[16];
extern unsigned short I;
extern unsigned short pc;
extern unsigned char sp;
extern unsigned short stack[16];
extern unsigned char keypad[16];
extern unsigned char display[64 * 32];
extern unsigned char delayTimer;
extern unsigned char soundTimer;


void initializeCHIP8(void);
int loadROM(char* filename);
void emulateCycle(void);

#define error(...) fprintf(stderr, __VA_ARGS__) //... are those va args thing
/* NOTE FOR SELF:
Variadic macros are macros that can accept a variable number of arguments.
The __VA_ARGS__ token is used to represent these arguments inside the macro definition.
When you define a macro with ... (an ellipsis) in the argument list, 
it means that the macro can take additional arguments beyond those explicitly listed.
 __VA_ARGS__ is then used within the macro to represent these extra arguments.
*/

#endif