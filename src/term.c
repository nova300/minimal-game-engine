#include "term.h"

SDL_Texture *term_font;

unsigned long timer = 0;

char* buffer = NULL;
char* framebuffer = NULL;

int cursorpos = 0;
int buffpos = 0;
int fblen = 0;
int bufflen = 0;
int buffalloc = 0;

int doClear = 0;
int cleared = 0;



int terminal_render(Terminal *term)
{
    timer += deltaTime;

    cursorpos = cursorpos % 799;

    if (framebuffer == NULL)
    {
        framebuffer = malloc(800);

        for (int i = 0; i < 800; i++)
        {
            framebuffer[i] = rand();
        }
        
        doClear = 800;

        return 0;
    }

    int k = 0;
    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 39; j++)
        {
            SDL_Rect glyph = {(16 * j) + 16 , (32 * i) + 16, 16, 32};
            SDL_Rect clip  = term_get_glyph(framebuffer[k]);
            SDL_RenderCopy(term->renderer, term_font, &clip, &glyph);
            k++;
        }
    }

    if (doClear)
    {
        if (timer < 25) return 0;
        
        for (int c = 0; c < 32; c++)
        {
            framebuffer[cleared] = 0;
            cleared++;
            if (cleared >= doClear) 
            {
                doClear = 0;
                timer = 0;
                return 0;
            } 
        }
        timer = 0;

        return 0;
    }

    if (timer < 15) return 0;
    if (buffpos >= bufflen) return 0;
    timer = 0;

    if (buffer[buffpos] == '\n') 
    {
        cursorpos = cursorpos + ( 39 - (cursorpos % 39));
        buffpos++;
    }
    framebuffer[cursorpos] = buffer[buffpos];
    buffpos++;
    cursorpos++;
}

SDL_Rect term_get_glyph(int code)
{
    //code = code - 31;
    SDL_Rect r = {8 * (code % 32), 16 * (((int) code / 32) % 8), 8, 16};
    return r;
}

int terminal_print(const char* str)
{
    if (bufflen > 800)
    {
        bufflen = bufflen % 800;
    }
    if ( buffpos >= bufflen)
    {
        bufflen = 0;
        buffpos = 0;
    }
    int len = strlen(str);
    if (buffer == NULL)
    {
        buffer = malloc(32);
        buffalloc = 32;
    }
    if (bufflen + len >= buffalloc)
    {
        buffer = realloc(buffer, buffalloc + 32 + len);
        buffalloc += 32 + len;
        printf("allocated %i bytes\n", buffalloc);
    }
    strcpy(buffer + bufflen, str);
    bufflen += len;
}

Terminal* terminal_new(SDL_Renderer *r)
{
    Terminal *s = malloc(sizeof(Terminal));
    Transform *t = (Transform*)s;
    t->position.x = 0;
    t->position.y = 0;
    t->position.z = 0;
    s->w = 0;
    s->h = 0;
    s->renderer = r;
    s->texture = NULL;
    return s;
}