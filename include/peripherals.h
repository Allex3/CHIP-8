#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

extern int quit;

void initDisplay();
void draw(unsigned char* display); //draw pixels using the display array
void sdlEventHandler(unsigned char* keypad); //use SDL events for keypad
void stopDisplay();

#endif