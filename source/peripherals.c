#include "peripherals.h"
#include <SDL2/SDL.h>

// initializez a SDL Window that holds and renders the display of the CHIP-8
// NOTE: use lazyfoo.net's guide on SDL2 :3
// tho I will use SDL_Renderer instead of SDL_Surface
/*
FROM STACKOVERFLAW:

 Essentially, SDL_Surface is a tool for CPU side blit rendering,
 where as SDL_Renderer takes advantage of hardware acceleration,
 and performs the rendering on the GPU
 There are very few reasons for using SDL_Surfaces for rendering these days

 SDL_Renderer is a struct that handles all rendering
 It is tied to a SDL_Window so it can only render within that SDL_Window
 It also keeps track the settings related to the rendering

 SDL_Renderer and its SDL_Texture is a much better performing choice
 if you don't need CPU side access to individual pixels.

 The SDL_Renderer renders SDL_Texture, which stores the pixel information of one element.
 SDL_Texture It's the new version of SDL_Surface which is much the same.
   The difference is mostly that SDL_Surface is just a struct containing pixel information,
    while SDL_Texture is an efficient, driver-specific representation of pixel data.


*/

// SDL_Window is the actual window that is the display

// The window we'll be rendering to
SDL_Window *window = NULL;

// The surface contained by the window
SDL_Renderer *renderer = NULL;

// SDL_Scancode is just an enum, and the interior are just numbers
// that in the SDL documentation mean actual keys that are pressed
SDL_Scancode keyMap[16] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V};

int quit = 0;

void initDisplay()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * 16, 32 * 16, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    /*flag means that the renderer uses hardware  acceleration */
}

void draw(unsigned char *display)
{
    // set color to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // clear the current rendering target with the drawing color set above
    SDL_RenderClear(renderer);

    // set color to white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // the display is black, so when pixels are set they're white
    // iterate through the display
    // the display is 64 x 8 width ,32 x 8 height, 8 because of the bits
    // and a pixel is a byte, the font for example is 5 bytes long

    // the display is as follows:
    // 0->63 is row 1 of pixels, 64->127 is row two, etc.
    //  the row is being represented by y*64
    // the pixel in that row, on the X axis, is literally x
    // so the current pixel targeted is x+y*64
    // it could also be a matrix display[y][x]
    // with y going from 0 to 63
    // but it's easier this way and it is literally the same
    for (int y = 0; y < 32; y++)
        for (int x = 0; x < 64; x++)
            if (display[x + (y * 64)]) // current pixel is set make it white
            {
                SDL_Rect pixel;
                //we make relative *16 it's size so we don't actually see 64x32
                //because that's way too small
                //we see 16*16 that area so we actually see something
                pixel.x = x * 16;
                pixel.y = y * 16;
                pixel.w = 16;
                pixel.h = 16;
                // draw the pixel at coordinates x*8 and y*8
                // because the widnows size is actually 32*8*64*8
                // and make it's width and heigh of 8 and 8

                SDL_RenderFillRect(renderer, &pixel); // put the pixel rectangle on the renderer
            }

    // update the window
    SDL_RenderPresent(renderer);
}

void sdlEventHandler(unsigned char *keypad) // handles SDL events for keypad
{
    SDL_Event e;

    if (SDL_PollEvent(&e))
    {
        // the current keyboard state (the pressed keys)
        // A array element with a value of 1 means that the key is pressed
        //  and a value of 0 means that it is not.
        // Indexes into this array are obtained by using SDL_Scancode values.
        // so we can use the keyMap defined above to check for any pressed key

        const Uint8 *state = SDL_GetKeyboardState(NULL);

        if (e.type == SDL_QUIT || state[SDL_SCANCODE_ESCAPE]) // user quitted
        {
            quit = 1;
            return;
        }

        // update the chip8 keypad with the current keys pressed and not pressed
        // that interest us, so only the 16 keys from the keyMap
        // and check their state if 1 or 0 with the state array
        for (int keycode = 0; keycode < 16; keycode++)
            keypad[keycode] = state[keyMap[keycode]];
    }
}

void stopDisplay() 
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}