#include "graphics.h"


int freeSprite(struct sprite *spr)
{
    if (spr->texture != NULL)
    {
        SDL_DestroyTexture(spr->texture);
        spr->texture = NULL;
        spr->h = 0;
        spr->w = 0;
    }
}

int renderSprite(struct sprite *spr, SDL_Rect *clip)
{
    if (spr->renderer == NULL) return 1;
    if (spr->texture == NULL) return 2;
    SDL_Rect r = {spr->x, spr->y, spr->w, spr->h};
    if (clip != NULL)
    {
        r.w = clip->w;
        r.h = clip->h;
    }
    SDL_RenderCopy( spr->renderer, spr->texture, clip, &r );
}

int positionSprite(int x, int y, struct sprite *spr)
{
    spr->x = x;
    spr->y = y;
}

int colourSprite(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, struct sprite *spr)
{
    SDL_SetTextureColorMod(spr->texture, red, green, blue);
    SDL_SetTextureAlphaMod(spr->texture, alpha);
}

SDL_Texture* loadTexture(const char *name, SDL_Renderer *rend)
{
    SDL_Texture *t = NULL;
    SDL_Surface *s = IMG_Load(name);
    if (s == NULL)
    {
        printf("error loading image\n");
    }
    else
    {
        t = SDL_CreateTextureFromSurface( rend, s);
    }
    if (t == NULL) printf("error creating texture\n");
    SDL_FreeSurface(s);
    return t;
}

int loadFromFile(const char* filename, struct sprite *spr)
{
    
    freeSprite(spr);
    
    SDL_Texture *t = NULL;
    SDL_Surface *s = IMG_Load(filename);
    if (s == NULL)
    {
        printf("error loading image\n");
        return 1;
    }

    SDL_SetColorKey(s, SDL_TRUE, SDL_MapRGB(s->format, 0, 0xff, 0xff));

    t = SDL_CreateTextureFromSurface(spr->renderer, s);
    if (t == NULL)
    {
        printf("error making texture\n");
        return 2;
    }

    spr->h = s->h;
    spr->w = s->w;

    SDL_FreeSurface(s);

    spr->texture = t;

    return 0;
}

struct sprite* newSprite()
{
    struct sprite *s = malloc(sizeof(struct sprite));
    s->x = 0;
    s->y = 0;
    s->w = 0;
    s->h = 0;
    s->renderer = NULL;
    s->texture = NULL;
    return s;
}

void deleteSprite(struct sprite *spr)
{
    free(spr);
}