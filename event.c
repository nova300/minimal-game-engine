#include "engine.h"

SDL_Event e;


int eventhandler()
{
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            exitLoop = 1;
        }
    }

    const Uint8* kstate = SDL_GetKeyboardState( NULL );

    if (kstate[ SDL_SCANCODE_UP])
    {
        terminal_print("up input\n");
    }
    else if (kstate[ SDL_SCANCODE_DOWN])
    {
        terminal_print("down input\n");
    }
    if (kstate[SDL_SCANCODE_LEFT])
    {
        terminal_print("left input\n");
    }
    else if (kstate[SDL_SCANCODE_RIGHT])
    {
        terminal_print("right input\n");
    }
}