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
        transform_move(0, -0.5, 0, s1);
    }
    else if (kstate[ SDL_SCANCODE_DOWN])
    {
        transform_move(0, 0.5, 0, s1);
    }
    if (kstate[SDL_SCANCODE_LEFT])
    {
        transform_move(-0.5, 0, 0, s1);
    }
    else if (kstate[SDL_SCANCODE_RIGHT])
    {
        transform_move(0.5, 0, 0, s1);
    }
}