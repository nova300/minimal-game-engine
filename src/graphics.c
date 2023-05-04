#include "graphics.h"
#include "engine.h"
#include "stdlib.h"


/* 3D math */

float radians(float dgr)
{
    float rad = dgr * 3.14 / 180;
    return rad;
}

void vector_normalize(vec3* v) 
{
	float sqr = v->x * v->x + v->y * v->y + v->z * v->z;
	if(sqr == 1 || sqr == 0)
		return;
	float invrt = 1.f/sqrt(sqr);
	v->x *= invrt;
	v->y *= invrt;
	v->z *= invrt;
}

float vector_dot(vec3 v1, vec3 v2) 
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vec3 vector_cross(vec3 v1, vec3 v2) 
{
	vec3 out = {{0}};
	out.x = v1.y*v2.z - v1.z*v2.y;
	out.y = v1.z*v2.x - v1.x*v2.z;
	out.z = v1.x*v2.y - v1.y*v2.x;
	return out;
}

vec3 vector_subtract(vec3 v1, vec3 v2)
{
    vec3 out = {{0}};
    out.x = v1.x - v2.x;
    out.y = v1.y - v2.y;
    out.z = v1.z - v2.z;
    return out;
}

mat4 matrix_perspective(float fovy, float aspect_ratio, float near_plane, float far_plane) 
{
	mat4 out = { { 0 } };

	float y_scale = (float)(1/cos(fovy * 3.14 / 360));
	float x_scale = y_scale / aspect_ratio;
	float frustum_length = far_plane - near_plane;

	out.x.x = x_scale;
	out.y.y = y_scale;
	out.z.z = -((far_plane + near_plane) / frustum_length);
	out.z.w = -1;
	out.w.z = -((2 * near_plane * far_plane) / frustum_length);
	
	return out;
}

mat4 matrix_lookAt(vec3 eye, vec3 center, vec3 up) 
{
    vec3 f = vector_subtract(center, eye);
	vector_normalize(&f);
	vec3 u = up;
    vector_normalize(&u);
	vec3 s = vector_cross(f, u);
	vector_normalize(&s);
	u = vector_cross(s, f);

	mat4 out = IDENTITY_MATRIX;
	out.x.x = s.x;
	out.y.x = s.y;
	out.z.x = s.z;

	out.x.y = u.x;
	out.y.y = u.y;
	out.z.y = u.z;

	out.x.z = -f.x;
	out.y.z = -f.y;
	out.z.z = -f.z;

	out.w.x = -vector_dot(s, eye);
	out.w.y = -vector_dot(u, eye);
	out.w.z =  vector_dot(f, eye);
	return out;
}

void matrix_rotateY(mat4* m, float angle) 
{
	mat4 rotation = IDENTITY_MATRIX;
	float sine = (float)sin(angle);
	float cosine = (float)cos(angle);
	
	rotation.x.x = cosine;
	rotation.z.x = sine;
	rotation.x.z = -sine;
	rotation.z.z = cosine;

	memcpy(m->m, matrix_multiply(m, &rotation).m, sizeof(m->m));
}

mat4 matrix_multiply(mat4* m1, mat4* m2) 
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

int loadShaders(const char *vertex_source, const char *fragment_source)
{
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    printf("GRAPHICS: Compiling Vertex Shader... ");
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
    else
    {
        printf("OK\n");
    }

    printf("GRAPHICS: Compiling Fragment Shader... ");
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
    else
    {
        printf("OK\n");
    }

    printf("GRAPHICS: linking program... ");
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
    else
    {
        printf("OK\n");
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
    vec3 *pos = &(t->position);
    pos->x = x;
    pos->y = y;
    pos->z = z;
}

int transform_move(float x, float y, float z, void *obj)
{
    Transform *t = (Transform*)obj;
    vec3 *pos = &(t->position);
    pos->x += x;
    pos->y += y;
    pos->z += z;
}




int loadTexture(const char *name, int *texture)
{
    SDL_Surface *s = IMG_Load(name);
    if (s == NULL)
    {
        printf("could not find image file\n");
        return 1;
    }

    GLuint Mode = 0;
    if(s->format->BytesPerPixel == 3) Mode = GL_RGB;
    if(s->format->BytesPerPixel == 4) Mode = GL_RGBA;

    if (Mode == 0)
    {
        printf("TEXTURE: %s : unsupported pixel format: %dbbp. trying to convert... ", name , s->format->BytesPerPixel);
        SDL_Surface *ptr = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_RGBA32, 0);
        if (ptr == NULL) 
        {
            printf("FAIL\n");
            return 2;
        }
        SDL_FreeSurface(s);
        s = ptr;
        Mode = GL_RGBA;
        printf("OK\n");
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, Mode, s->w, s->h, 0, Mode, GL_UNSIGNED_BYTE, s->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    SDL_FreeSurface(s);

    *texture = textureID;
    return 0;
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
    /*SDL_Rect r = {t->x, t->y, spr->w, spr->h};
    if (clip != NULL)
    {
        r.w = clip->w;
        r.h = clip->h;
    }
    SDL_RenderCopyEx( spr->renderer, spr->texture, clip, &r , angle, center, flip);*/
}

int sprite_colour(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, Sprite *spr)
{
    SDL_SetTextureColorMod(spr->texture, red, green, blue);
    SDL_SetTextureAlphaMod(spr->texture, alpha);
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
    t->position.x = 0;
    t->position.y = 0;
    t->position.z = 0;
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

            SDL_Rect p = {ps->particles[i].transform.position.x, ps->particles[i].transform.position.y, ps->w, ps->h};

            SDL_RenderCopy( ps->renderer, ps->texture, NULL, &p);

        }
    }
}

ParticleSystem* particle_new(SDL_Renderer *r)
{
    ParticleSystem *ps = malloc(sizeof(ParticleSystem));
    Transform *t = (Transform*)ps;
    Particle *p = malloc(sizeof(Particle) * 64);
    t->position.x = 0;
    t->position.y = 0;
    t->position.z = 0;
    ps->renderer = r;
    ps->texture = NULL;

    
    for (int i = 0; i < 64; i++)
    {
        Transform *pt = (Transform*)&p[i];
        p[i].lifeTime = 0;
        p[i].xdir = 0;
        p[i].ydir = 0;
        pt->position.x = 0;
        pt->position.y = 0;
        pt->position.z = 0;
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