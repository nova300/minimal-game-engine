#ifndef graphics_h
#define graphics_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "shapes.h"


#define GOBJ_TYPE_SINGLE 1
#define GOBJ_TYPE_MULTI 2

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
vec3 vector_add(vec3 v1, vec3 v2);
vec3 vector_scale(vec3 v1, float s);

void matrix_rotateY(mat4* m, float angle);
mat4 matrix_multiply(mat4* m1, mat4* m2);
mat4 matrix_lookAt(vec3 eye, vec3 center, vec3 up);
mat4 matrix_perspective(float fovy, float aspect_ratio, float near_plane, float far_plane);

float radians(float dgr);
int FloatEquals(float a, float b, float floatEqualityThreshold);

typedef struct
{
    GLuint ShaderID;
    int ProjectionID;
    int ViewID;
}Shader;

GLuint loadShaders(const char *vertex_source, const char *fragment_source);
Shader* newShaderObject(const char *vertex_source, const char *fragment_source);

typedef struct
{
    mat4 matrix;
    vec4 position;
    vec4 rotation;
    vec4 scale;
}Transform;

void transform_position(float x, float y, float z, Transform *t);
void transform_move(float x, float y, float z, Transform *t);
void transform_set_identity(Transform *t);
void transform_set_rotation(float x, float y, float z, Transform *t);
void transform_rotate(float x, float y, float z, Transform *t);
void transform_make_matrix(Transform *t); /* updates matrix in transform struct to reflect its other properties */

extern mat4 projectionMatrix;
extern mat4 viewMatrix;

typedef  struct 
{
    unsigned int  count;
    unsigned int  instanceCount;
    unsigned int  firstIndex;
    int  baseVertex;
    unsigned int  baseInstance;
} drawCommand;

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
    unsigned char type;
    unsigned int count;
    unsigned int indexCount;
    Shader *shader;
    GLuint textureAtlas;
    GLuint vertexBuffer;
    GLuint transformBuffer;
    GLuint textureBuffer;
    GLuint elementBuffer;
    GLuint commandBuffer;
}GeoObject_gpu_handle;

typedef struct
{
    Transform baseTransform;
    int baseTexture;
    vertex *data;
    int dataCount;
    int *indicies;
    int indexCount;
    int instanceCount;
    int instanceCapacity;
    mat4 *transform;
    int *texture;
    char dataDirty;
    char instanceDirty;
}GeoObject;

typedef struct
{
    GeoObject_gpu_handle gpuHandle;
    GeoObject geoObject;
}GeoObject_gpu;

typedef struct
{
    GeoObject_gpu_handle gpuHandle;
    unsigned int count;
    unsigned int capacity;
    GeoObject **objectBuffer;
}RenderQueue;

RenderQueue *rq_new_queue(int capacity);
void rq_update_buffers(RenderQueue *rq);
void rq_add_object(RenderQueue *rq, GeoObject *obj);
void rq_init(RenderQueue *rq, int capacity);

void geo_obj_gpu_handle_genBuffers(GeoObject_gpu_handle *gpuHandle, unsigned int type);
GeoObject_gpu *geo_obj_bindToGpu(GeoObject obj);
GeoObject_gpu *geo_obj_bindToGpu_and_free(GeoObject *obj);
void geo_obj_gpu_updateBuffers(GeoObject_gpu *obj);

GLuint loadTexture(const char *name);
GLuint generateColorTexture(float r, float g, float b, float a);
GLuint generateRandomAtlas(void);
int geo_obj_loadFromFile(const char* filename, GeoObject *obj);
int geo_mdl_loadFromFile(const char* filename, GeoObject *obj);

GeoObject *geo_obj_createFromParShape(par_shapes_mesh* mesh);

int geo_obj_createObjectData(GeoObject *obj, vec3* vertices, vec2* uvs, vec3* normals, int vertexCount, float floatEqualityThreshold);

void geo_render(GeoObject_gpu_handle *obj);
void geo_render_translated(GeoObject_gpu *obj, Transform *t);
void geo_render_multi(RenderQueue *rq);
GeoObject *geo_new_object(void);

void geo_instanceop_init(GeoObject *obj, int capacity);
void geo_instanceop_free(GeoObject *obj);
void geo_instanceop_resize(GeoObject *obj, int newCapacity);
void geo_instanceop_add(GeoObject *obj, mat4 matrix, int textureIndex);
void geo_instanceop_remove(GeoObject *obj, int index);
void geo_instanceop_clear(GeoObject *obj);

typedef struct
{
    Transform transform;
    float lifeTime;
    float ydir;
    float xdir;
    float zdir;
    int texture;
}Particle;

typedef struct
{
    Transform *transform;
    Particle *particles;
    int amount;
    GeoObject *geo;
}ParticleSystem;

ParticleSystem* particle_new(GeoObject *g, int amount);
void particle_update(ParticleSystem *ps);

void fb_init();
void fb_update();
void fb_render();
void fb_clear();
void fb_drawSineWave(int amplitude, float frequency, float of);

#endif