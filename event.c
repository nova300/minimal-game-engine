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
        degrees += 0.05;
        printf("degrees %f\n", degrees);
    }
    else if (kstate[ SDL_SCANCODE_DOWN])
    {
        degrees -= 0.05;
        printf("degrees %f\n", degrees);
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