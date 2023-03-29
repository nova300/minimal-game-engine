#include "term.h"

unsigned long timer = 0;

char* buffer = NULL;
char* framebuffer = NULL;

int i = 0;
int fblen = 0;
int bufflen = 0;
int buffalloc = 0;



int terminal_render(Terminal *term)
{
    timer += deltaTime;
    if (i < bufflen && timer > 75)
    {
        timer = 0;
        framebuffer[i] = buffer[i];
        framebuffer[i + 1] = '\0';
        i++;

        SDL_DestroyTexture(term->texture);
        SDL_Color tcolor = {255, 255, 255};
        SDL_Texture *tex = NULL;
        SDL_Surface *s = TTF_RenderUTF8_Solid_Wrapped( font, framebuffer, tcolor, 0);
        term->texture = SDL_CreateTextureFromSurface(term->renderer, s);
        term->w = s->w;
        term->h = s->h;
        SDL_FreeSurface(s);
    }

    if (term->renderer == NULL) return 1;
    if (term->texture == NULL) return 2;
    Transform *tra = (Transform*)term;
    SDL_Rect r = {tra->x, tra->y, term->w, term->h};
    SDL_RenderCopy( term->renderer, term->texture, NULL, &r);
}

int term_print(const char* str)
{
    if ( i >= bufflen)
    {
        bufflen = 0;
    }
    int len = strlen(str);
    if (framebuffer == NULL)
    {
        framebuffer = malloc(255);
        fblen = 0;
        framebuffer[fblen] = '\0';
    }
    if (buffer == NULL)
    {
        buffer = malloc(32);
        buffalloc = 32;
    }
    if (bufflen + len >= buffalloc)
    {
        buffer = realloc(buffer, buffalloc + 32 + len);
        buffalloc += 32 + len;
    }
    strcpy(buffer + bufflen, str);
    bufflen += len;
}

Terminal* terminal_new(SDL_Renderer *r)
{
    Terminal *s = malloc(sizeof(Terminal));
    Transform *t = (Transform*)s;
    t->x = 0;
    t->y = 0;
    t->z = 0;
    s->w = 0;
    s->h = 0;
    s->renderer = r;
    s->texture = NULL;
    return s;
}