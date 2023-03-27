#ifndef graphics_h
#define graphics_h


#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct sprite
{
    float x;
    float y;
    float z;
    int w;
    int h;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
};

int loadFromFile(const char* filename, struct sprite *spr);
int freeSprite(struct sprite *spr);
int renderSprite(struct sprite *spr, SDL_Rect *clip);
int positionSprite(int x, int y, struct sprite *spr);
int colourSprite(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, struct sprite *spr);
SDL_Texture* loadTexture(const char *name, SDL_Renderer *rend);
struct sprite* newSprite();
void deleteSprite(struct sprite *spr);


#endif