#include "engine.h"

unsigned long time = 0;
unsigned long deltaTime = 0;

SDL_Window *window;
SDL_Renderer *rend;

TTF_Font *font;

ParticleSystem *p1;

int exitLoop = 0;

SDL_RendererFlip flipType = SDL_FLIP_NONE;
float degrees = 0;


int ENTRYPOINT(int argc, char *argv[])
{
    printf("init: ");
    terminal_print("init: ");
    int err = init();
    if (err != 0)
    {
        err = err + 100;
        printf("error %i\n", err);
    }
    printf("ok\n");
    terminal_print("ok\n");

    font = TTF_OpenFont("media/tosh.ttf", 28);
    if (font == NULL)
    {
        printf("failed to open font\n");
    }
    term_font = loadTexture("media/font_1.png", rend);
    p1 = particle_new(rend);
    printf("test\n");
    SDL_Color tcolor = { 0xff, 0xff, 0xff, 0xff};
    particle_loadFromRenderedText("*", tcolor, p1);
    
    transform_position((SCREEN_WIDTH - p1->w) / 2, (SCREEN_HEIGHT - p1->h) / 2, 0, p1);
    

    Terminal *t1 = terminal_new(rend);
    terminal_print("hello world!!\n");

    while (exitLoop == 0)
    {

        deltaTime = SDL_GetTicks() - time;
        eventhandler();
        //if (deltaTime < 1000) continue;
        if (deltaTime > 250) deltaTime = 250;
        time = SDL_GetTicks();
        SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(rend);

        particle_render(p1);
        
        terminal_render(t1);

        //SDL_Rect r = {0, 0, SCREEN_HEIGHT, SCREEN_WIDTH};
        //SDL_Texture *t = term_get(rend);
        //SDL_RenderCopy(rend, t, NULL, NULL);
        //SDL_DestroyTexture(t);

        SDL_RenderPresent(rend);
    }


    quit();
    printf("Goodbye.\n");
    return 0;
}

void helloworld()
{
    printf("hello world");
    return;
}