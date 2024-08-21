#include <stdio.h>


#include "peripherals.h"
#include "chip8.h"

#define true 1
#define false 0

int main(int argc, char *args[])
{
    if (argc!=2)
    {
        fprintf(stderr, "Usage: emulator rom.ch8");
        return 1;
    }    

    printf("Initiliazing CHIP-8\n");
    initializeCHIP8();
    printf("Done.\n");

    //Load ROM (not yet have roms oops)

    puts("ROM Loaded Successfully");

    initDisplay(); //Initialize the display
    puts("Display loaded successfully");

    while (!quit) 
    {
        emulateCycle();
        sdlEventHandler(keypad); //check what keys are pressed and do actions accordingly
        

        if (V[0xF]) //draw flag set
            draw(display); //update the display
        
    }

    stopDisplay();
    return 0;
}