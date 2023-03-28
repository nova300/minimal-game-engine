#include "engine.h"
#include "graphics.h"
#include "ray.h"

SDL_Window *window;
SDL_Renderer *rend;

int exitLoop = 0;


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

    sprite *bg = sprite_new();
    bg->renderer = rend;
    sprite_loadFromFile("bg.png", bg);

    while (exitLoop == 0)
    {
        eventhandler();
        SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(rend);


        //renderSprite(bg, NULL);
        SDL_Rect r = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_Rect c = {0, 0, 900, 600};
        //SDL_RenderCopy(rend, bg.texture, NULL, &r);

        SDL_Texture *t = raycast(rend);

        SDL_RenderCopy(rend, t, &c, &r);

        SDL_DestroyTexture(t);

        SDL_RenderPresent(rend);
    }

    sprite_delete(bg);

    quit();
    printf("Goodbye.\n");
    return 0;
}

void helloworld()
{
    printf("hello world");
    return;
}