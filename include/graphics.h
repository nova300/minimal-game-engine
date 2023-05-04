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
	    float x;
        float y;
        float z;
        float w;
	};
}vec4;

typedef union
{
    float m[3];
    struct
    {
        float x;
        float y;
        float z;
    };
}vec3;

typedef union
{
    float m[2];
    struct
    {
        float x;
        float y;
    };
    
}vec2;

typedef union
{
	float m[16];
    struct
    {
        vec4 x;
        vec4 y;
        vec4 z;
        vec4 w;
    };
    
}mat4;


static const mat4 IDENTITY_MATRIX = 
{{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
}};


float vector_dot(vec4 v1, vec4 v2);
vec4 vector_cross(vec4 v1, vec4 v2);
void vector_normalize(vec4* v);
vec4 vector_subtract(vec4 v1, vec4 v2);

void matrix_rotateY(mat4* m, float angle);
mat4 matrix_multiply(mat4* m1, mat4* m2);
mat4 matrix_lookAt(vec4 eye, vec4 center, vec4 up);
mat4 matrix_perspective(float fovy, float aspect_ratio, float near_plane, float far_plane);

float radians(float dgr);


int loadShaders(const char *vertex_source, const char *fragment_source);

typedef struct
{
    vec4 position;
    vec4 rotation;
    vec4 scale;
}Transform;

int transform_position(float x, float y, float z, void *obj);
int transform_move(float x, float y, float z, void *obj);

typedef struct
{
    vec3 *vertexBuffer;
    vec2 *uvBuffer;
    int bufferLength;
    int triCount;
    int texture;
    mat4 PRO;
    mat4 VIE;
    mat4 MOD;
}GeoObject;

int loadTexture(const char *name, int *texture);

int geo_obj_loadFromFile(const char* filename, GeoObject *obj);
int geo_mdl_loadFromFile(const char* filename, GeoObject *obj);


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