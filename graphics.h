#ifndef graphics_h
#define graphics_h


#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct 
{
    float x;
    float y;
    int w;
    int h;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
}sprite;

int sprite_loadFromFile(const char* filename, sprite *spr);
int sprite_free(sprite *spr);
int sprite_render(sprite *spr, SDL_Rect *clip);
int sprite_position(int x, int y, sprite *spr);
int sprite_colour(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, sprite *spr);
sprite* sprite_new();
void sprite_delete(sprite *spr);


SDL_Texture* loadTexture(const char *name, SDL_Renderer *rend);


#endif