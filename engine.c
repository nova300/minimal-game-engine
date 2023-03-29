#include "engine.h"

unsigned long time = 0;
unsigned long deltaTime = 0;

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
    term_print("init: ");
    int err = init();
    if (err != 0)
    {
        err = err + 100;
        printf("error %i\n", err);
    }
    printf("ok\n");
    term_print("ok\n");

    font = TTF_OpenFont("tosh.ttf", 28);
    if (font == NULL)
    {
        printf("failed to open font\n");
    }
    s1 = sprite_new(rend);
    SDL_Color tcolor = { 255, 255, 255};
    sprite_loadFromRenderedText( "hello world", tcolor, s1);
    transform_position((SCREEN_WIDTH - s1->w) / 2, (SCREEN_HEIGHT - s1->h) / 2, 0, s1);

    Terminal *t1 = terminal_new(rend);
    term_print("hello world!!\n");

    while (exitLoop == 0)
    {

        deltaTime = SDL_GetTicks() - time;
        eventhandler();
        //if (deltaTime < 1000) continue;
        if (deltaTime > 250) deltaTime = 250;
        time = SDL_GetTicks();
        SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(rend);

        //sprite_render(s1, NULL, degrees, NULL, flipType);
        
        terminal_render(t1);

        //SDL_Rect r = {0, 0, SCREEN_HEIGHT, SCREEN_WIDTH};
        //SDL_Texture *t = term_get(rend);
        //SDL_RenderCopy(rend, t, NULL, NULL);
        //SDL_DestroyTexture(t);

        degrees = degrees + 0.1 * deltaTime;

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