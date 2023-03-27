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
        move(0);
    }
    else if (kstate[ SDL_SCANCODE_DOWN])
    {
        move(1);
    }
    if (kstate[SDL_SCANCODE_LEFT])
    {
        pan(0);
    }
    else if (kstate[SDL_SCANCODE_RIGHT])
    {
        pan(1);
    }
}