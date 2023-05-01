#include "graphics.h"
#include "engine.h"


/* GL Functions */

int LoadShaders(const char *vertex_source, const char *fragment_source)
{
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    printf("Compiling Vertex Shader\n");
    glShaderSource(VertexShaderID, 1, &vertex_source, NULL);
    glCompileShader(VertexShaderID);

    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        char *errmsg = malloc(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, errmsg);
        printf("%s\n", errmsg);
        free(errmsg);
    }

    printf("Compiling Fragment Shader\n");
    glShaderSource(FragmentShaderID, 1, &fragment_source, NULL);
    glCompileShader(FragmentShaderID);

    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        char *errmsg = malloc(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, errmsg);
        printf("%s\n", errmsg);
        free(errmsg);
    }

    printf("linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        char *errmsg = malloc(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, errmsg);
        printf("%s\n", errmsg);
        free(errmsg);
    }

    glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}






/* Graphics Objects */

int transform_position(float x, float y, float z, void *obj)
{
    Transform *t = (Transform*)obj;
    t->x = x;
    t->y = y;
    t->z = z;
}

int transform_move(float x, float y, float z, void *obj)
{
    Transform *t = (Transform*)obj;
    t->x += x;
    t->y += y;
    t->z += z;
}

int sprite_free(Sprite *spr)
{
    if (spr->texture != NULL)
    {
        SDL_DestroyTexture(spr->texture);
        spr->texture = NULL;
        spr->h = 0;
        spr->w = 0;
    }
}

int sprite_render(Sprite *spr, SDL_Rect *clip, float angle, SDL_Point *center, SDL_RendererFlip flip)
{
    if (spr->renderer == NULL) return 1;
    if (spr->texture == NULL) return 2;
    Transform *t = (Transform*)spr;
    SDL_Rect r = {t->x, t->y, spr->w, spr->h};
    if (clip != NULL)
    {
        r.w = clip->w;
        r.h = clip->h;
    }
    SDL_RenderCopyEx( spr->renderer, spr->texture, clip, &r , angle, center, flip);
}

int sprite_colour(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, Sprite *spr)
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

int sprite_loadFromFile(const char* filename, Sprite *spr)
{
    
    sprite_free(spr);
    
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

Sprite* sprite_new(SDL_Renderer *r)
{
    Sprite *s = malloc(sizeof(Sprite));
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

void sprite_delete(Sprite *spr)
{
    sprite_free(spr);
    free(spr);
}

#if defined(SDL_TTF_MAJOR_VERSION)
int sprite_loadFromRenderedText(const char* text, SDL_Color textColor, Sprite *spr)
{
    sprite_free(spr);
    SDL_Surface *s = TTF_RenderText_Solid( font, text, textColor);
    if( s == NULL )
    {
        printf("could not create text\n");
        return 1;
    }
    
    spr->texture = SDL_CreateTextureFromSurface(spr->renderer, s);

    if (spr->texture == NULL)
    {
        printf("could not create texture from text surface\n");
        return 2;
    }

    spr->w = s->w;
    spr->h = s->h;

    SDL_FreeSurface(s);

    return 0;
}
#endif

int particle_render(ParticleSystem *ps)
{
    for (int i = 0; i < 64; i++)
    {
        if (ps->particles[i].lifeTime < 0)
        {
            
            ps->particles[i].lifeTime = (rand() % 500) + 250;
            ps->particles[i].transform = ps->transform;
            ps->particles[i].xdir = ((rand() - rand()) % 3) + ((rand() - rand()) % 10) * 0.1f;
            ps->particles[i].ydir = ((rand() - rand()) % 3) + ((rand() - rand()) % 10) * 0.1f;;
        }
        else
        {
            ps->particles[i].lifeTime = ps->particles[i].lifeTime - (deltaTime);
            transform_move(ps->particles[i].xdir * (deltaTime*0.2), ps->particles[i].ydir * (deltaTime*0.2), 0, &ps->particles[i]);

            SDL_Rect p = {ps->particles[i].transform.x, ps->particles[i].transform.y, ps->w, ps->h};

            SDL_RenderCopy( ps->renderer, ps->texture, NULL, &p);

        }
    }
}

ParticleSystem* particle_new(SDL_Renderer *r)
{
    ParticleSystem *ps = malloc(sizeof(ParticleSystem));
    Transform *t = (Transform*)ps;
    Particle *p = malloc(sizeof(Particle) * 64);
    t->x = 0;
    t->y = 0;
    t->z = 0;
    ps->renderer = r;
    ps->texture = NULL;

    
    for (int i = 0; i < 64; i++)
    {
        Transform *pt = (Transform*)&p[i];
        p[i].lifeTime = 0;
        p[i].xdir = 0;
        p[i].ydir = 0;
        pt->x = 0;
        pt->y = 0;
        pt->z = 0;
    }
    
    ps->particles = p;

    return ps;
}

int particle_loadFromFile(const char* filename, ParticleSystem *ps)
{
    
    if (ps->texture != NULL)
    {
        SDL_DestroyTexture(ps->texture);
    }
    
    SDL_Texture *t = NULL;
    SDL_Surface *s = IMG_Load(filename);
    if (s == NULL)
    {
        printf("error loading image\n");
        return 1;
    }

    SDL_SetColorKey(s, SDL_TRUE, SDL_MapRGB(s->format, 0, 0xff, 0xff));

    t = SDL_CreateTextureFromSurface(ps->renderer, s);
    if (t == NULL)
    {
        printf("error making texture\n");
        return 2;
    }

    ps->h = s->h;
    ps->w = s->w;

    SDL_FreeSurface(s);

    ps->texture = t;

    return 0;
}

int particle_loadFromRenderedText(const char* text, SDL_Color textColor, ParticleSystem *ps)
{
    if (ps->texture != NULL)
    {
        SDL_DestroyTexture(ps->texture);
        ps->texture = NULL;
    }
    SDL_Surface *s = TTF_RenderText_Solid( font, text, textColor);
    if( s == NULL )
    {
        printf("could not create text\n");
        return 1;
    }
    
    ps->texture = SDL_CreateTextureFromSurface(ps->renderer, s);

    if (ps->texture == NULL)
    {
        printf("could not create texture from text surface\n");
        return 2;
    }

    ps->w = s->w;
    ps->h = s->h;

    SDL_FreeSurface(s);

    return 0;
}