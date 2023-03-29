#ifndef term_h
#define term_h

#include "engine.h"
#include <string.h>

typedef struct
{
    Transform transform;
    int w;
    int h;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
}Terminal;


int terminal_render(Terminal* term);
int terminal_print(const char* str);
Terminal* terminal_new(SDL_Renderer *r);

#endif