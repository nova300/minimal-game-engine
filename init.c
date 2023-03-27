#include "engine.h"

int init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return 1;
    }

    window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        return 2;
    }

    rend = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (rend == NULL)
    {
        return 3;
    }

    
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        return 4;
    }
    

    SDL_SetRenderDrawColor( rend, 0xff, 0xff, 0xff, 0xff);

    return 0;
}


void quit()
{
    SDL_DestroyRenderer( rend );

    SDL_DestroyWindow( window );
    IMG_Quit();
    SDL_Quit();
}

