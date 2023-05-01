#include "graphics.h"
#include "engine.h"


/* 3D math */

void normalizevec4(vec4* v) 
{
	float sqr = v->m[0] * v->m[0] + v->m[1] * v->m[1] + v->m[2] * v->m[2];
	if(sqr == 1 || sqr == 0)
		return;
	float invrt = 1.f/sqrt(sqr);
	v->m[0] *= invrt;
	v->m[1] *= invrt;
	v->m[2] *= invrt;
}

float dotvec4(vec4 v1, vec4 v2) 
{
	return v1.m[0] * v2.m[0] + v1.m[1] * v2.m[1] + v1.m[2] * v2.m[2] + v1.m[3] * v2.m[3];
}

vec4 crossvec4(vec4 v1, vec4 v2) 
{
	vec4 out = {{0}};
	out.m[0] = v1.m[1]*v2.m[2] - v1.m[2]*v2.m[1];
	out.m[1] = v1.m[2]*v2.m[0] - v1.m[0]*v2.m[2];
	out.m[2] = v1.m[0]*v2.m[1] - v1.m[1]*v2.m[0];
	return out;
}

mat4 perspective(float fovy, float aspect_ratio, float near_plane, float far_plane) 
{
	mat4 out = { { 0 } };

	const float
		y_scale = (float)(1/cos(fovy * 3.14 / 360)),
		x_scale = y_scale / aspect_ratio,
		frustum_length = far_plane - near_plane;

	out.m[0] = x_scale;
	out.m[5] = y_scale;
	out.m[10] = -((far_plane + near_plane) / frustum_length);
	out.m[11] = -1;
	out.m[14] = -((2 * near_plane * far_plane) / frustum_length);
	
	return out;
}

mat4 lookAt(vec4 pos, vec4 dir) 
{
	vec4 f = dir;
	normalizevec4(&f);
	vec4 u = {{0, 1, 0, 0}};
	vec4 s = crossvec4(f, u);
	normalizevec4(&s);
	u = crossvec4(s, f);

	mat4 out = IDENTITY_MATRIX;
	out.m[0] = s.x;
	out.m[4] = s.y;
	out.m[8] = s.z;

	out.m[1] = u.x;
	out.m[5] = u.y;
	out.m[9] = u.z;

	out.m[2] = -f.x;
	out.m[6] = -f.y;
	out.m[10] = -f.z;

	out.m[12] = -dotvec4(s, pos);
	out.m[13] = -dotvec4(u, pos);
	out.m[14] =  dotvec4(f, pos);
	return out;
}

void rotateY(const mat4* m, float angle) 
{
	mat4 rotation = IDENTITY_MATRIX;
	float sine = (float)sin(angle);
	float cosine = (float)cos(angle);
	
	rotation.m[0] = cosine;
	rotation.m[8] = sine;
	rotation.m[2] = -sine;
	rotation.m[10] = cosine;

	memcpy(m->m, multiplymat4(m, &rotation).m, sizeof(m->m));
}

mat4 multiplymat4(const mat4* m1, const mat4* m2) 
{
	mat4 out = IDENTITY_MATRIX;
	unsigned int row, column, row_offset;

	for (row = 0, row_offset = row * 4; row < 4; ++row, row_offset = row * 4)
		for (column = 0; column < 4; ++column)
			out.m[row_offset + column] =
				(m1->m[row_offset + 0] * m2->m[column + 0]) +
				(m1->m[row_offset + 1] * m2->m[column + 4]) +
				(m1->m[row_offset + 2] * m2->m[column + 8]) +
				(m1->m[row_offset + 3] * m2->m[column + 12]);

	return out;
}

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