#include "engine.h"


SDL_Window *window;
SDL_Renderer *rend;

TTF_Font *font;

Sprite *s1;

int exitLoop = 0;

SDL_RendererFlip flipType = SDL_FLIP_NONE;
float degrees = 0;


int ENTRYPOINT(int argc, char *argv[])
{
    printf("init: ");
    int err = init();
    if (err != 0)
    {
        err = err + 100;
        printf("error %i\n", err);
    }
    printf("ok\n");

    font = TTF_OpenFont("tosh.ttf", 28);
    if (font == NULL)
    {
        printf("failed to open font\n");
    }

    s1 = sprite_new(rend);
    SDL_Color tcolor = { 255, 255, 255};
    sprite_loadFromRenderedText( "hello world", tcolor, s1);
    transform_position((SCREEN_WIDTH - s1->w) / 2, (SCREEN_HEIGHT - s1->h) / 2, 0, s1);

    while (exitLoop == 0)
    {
        eventhandler();
        SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(rend);

        sprite_render(s1, NULL, degrees, NULL, flipType);
        
        /*
        SDL_Rect r = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_Rect c = {0, 0, 900, 600};
        SDL_Texture *t = raycast(rend);
        SDL_RenderCopy(rend, t, &c, &r);
        SDL_DestroyTexture(t);
        */

        SDL_RenderPresent(rend);
    }

    sprite_delete(s1);

    quit();
    printf("Goodbye.\n");
    return 0;
}

void helloworld()
{
    printf("hello world");
    return;
}