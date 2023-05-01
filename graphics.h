#ifndef graphics_h
#define graphics_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


extern TTF_Font *font;

typedef union
{
	 float m[4];
	 struct 
     {
	 	float x, y, z, w;
	 };
}vec4;

typedef struct
{
	float m[16];
}mat4;

static const mat4 IDENTITY_MATRIX = 
{{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
}};

mat4 multiplymat4(const mat4* m1, const mat4* m2);
mat4 lookAt(vec4 pos, vec4 dir);
mat4 perspective(float fovy, float aspect_ratio, float near_plane, float far_plane);
float dotvec4(vec4 v1, vec4 v2);
vec4 crossvec4(vec4 v1, vec4 v2);
void normalizevec4(vec4* v);
void rotateY(const mat4* m, float angle);


int LoadShaders(const char *vertex_source, const char *fragment_source);

SDL_Texture* loadTexture(const char *name, SDL_Renderer *rend);

typedef struct
{
    float x;
    float y;
    float z;
    int id;
}Transform;

int transform_position(float x, float y, float z, void *obj);
int transform_move(float x, float y, float z, void *obj);

typedef struct 
{
    Transform transform;
    int w;
    int h;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
}Sprite;

int sprite_loadFromFile(const char* filename, Sprite *spr);
int sprite_free(Sprite *spr);
int sprite_render(Sprite *spr, SDL_Rect *clip, float angle, SDL_Point *center, SDL_RendererFlip flip);
int sprite_colour(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, Sprite *spr);
Sprite* sprite_new(SDL_Renderer *r);
void sprite_delete(Sprite *spr);
int sprite_loadFromRenderedText(const char* text, SDL_Color textColor, Sprite *spr);


typedef struct
{
    Transform transform;
    int lifeTime;
    float ydir;
    float xdir;

}Particle;

typedef struct
{
    Transform transform;
    Particle *particles;
    int w;
    int h;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
}ParticleSystem;

ParticleSystem* particle_new(SDL_Renderer *r);
int particle_loadFromFile(const char* filename, ParticleSystem *ps);
int particle_loadFromRenderedText(const char* text, SDL_Color textColor, ParticleSystem *ps);
int particle_render(ParticleSystem *ps);






#endif