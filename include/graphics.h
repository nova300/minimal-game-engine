#ifndef graphics_h
#define graphics_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "shapes.h"

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


float vector_dot(vec3 v1, vec3 v2);
vec3 vector_cross(vec3 v1, vec3 v2);
void vector_normalize(vec3* v);
vec3 vector_subtract(vec3 v1, vec3 v2);

void matrix_rotateY(mat4* m, float angle);
mat4 matrix_multiply(mat4* m1, mat4* m2);
mat4 matrix_lookAt(vec3 eye, vec3 center, vec3 up);
mat4 matrix_perspective(float fovy, float aspect_ratio, float near_plane, float far_plane);

float radians(float dgr);
int FloatEquals(float a, float b, float floatEqualityThreshold);

typedef struct
{
    int ShaderID;
    int ProjectionID;
    int ModelID;
    int ViewID;
    int ColorID;
}Shader;

int loadShaders(const char *vertex_source, const char *fragment_source);
Shader* newShaderObject(const char *vertex_source, const char *fragment_source);

typedef struct
{
    mat4 *matrix;
    vec4 position;
    vec4 rotation;
    vec4 scale;
}Transform;

typedef struct
{
    mat4 *data;
    int capacity;
    int count;
}TransformArray;

int transform_position(float x, float y, float z, Transform *t);
int transform_move(float x, float y, float z, Transform *t);
int transform_set_identity(Transform *t);
int transform_set_rotation(float x, float y, float z, Transform *t);
int transform_rotate(float x, float y, float z, Transform *t);
int transform_make_matrix(Transform *t); /* updates matrix in transform struct to reflect its other properties */

extern int vertexBuffer;
extern int transformBuffer;
extern int elementBuffer;

extern mat4 projectionMatrix;
extern mat4 viewMatrix;

typedef union
{
    float v[8];
    struct
    {
        vec3 vertex;
        vec3 normal;
        vec2 uv;
    };
    
}vertex;

typedef struct
{
    Transform transform;
    TransformArray transformArray;
    vertex *data;
    int dataCount;
    int *indicies;
    int indexCount;
    int texture;
    vec3 color;
    Shader *shader;
}GeoObject;



int loadTexture(const char *name, int *texture);
int geo_obj_loadFromFile(const char* filename, GeoObject *obj);
int geo_mdl_loadFromFile(const char* filename, GeoObject *obj);

GeoObject *geo_obj_createFromParShape(par_shapes_mesh* mesh);

int geo_obj_createObjectData(GeoObject *obj, vec3* vertices, vec2* uvs, vec3* normals, int vertexCount, float floatEqualityThreshold);

int geo_render(GeoObject *obj);
int geo_render_translated(GeoObject *obj, Transform *t);
GeoObject *geo_new_object();


void transformArray_init(TransformArray *obj, int capacity, int count);
void transformArray_free(TransformArray *obj);
void transformArray_resize(TransformArray *obj, int newCapacity);
void transformArray_add(TransformArray *obj, mat4 matrix);
void transformArray_remove(TransformArray *obj, int index);
void transformArray_clear(TransformArray *obj);

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
    float zdir;
    vec3 color;
}Particle;

typedef struct
{
    Transform *transform;
    Particle *particles;
    int amount;
    GeoObject *geo;
}ParticleSystem;

ParticleSystem* particle_new(GeoObject *g, int amount);
int particle_render(ParticleSystem *ps);
int particle_render_colorful(ParticleSystem *ps);






#endif