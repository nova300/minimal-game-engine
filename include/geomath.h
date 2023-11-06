#ifndef geomath_h
#define geomath_h

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
    float m[3];
    struct
    {
        float x;
        float y;
        float z;
    };
    vec2 xy;
}vec3;

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
    struct 
    {
        vec2 xy;
        vec2 zw;
	};
    vec3 xyz;
}vec4;

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

typedef struct
{
    int x, y;
    int w, h;
}rect;

static const mat4 IDENTITY_MATRIX = 
{{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
}};


float vector_dot(vec4 *v1, vec4 *v2);
vec4 vector_cross(vec4 v1, vec4 v2);
void vector_normalize(vec4* v);
vec4 vector_subtract(vec4 v1, vec4 v2);
vec4 vector_add(vec4 v1, vec4 v2);
vec4 vector_scale(vec4 v1, float s);
float vector_distance(vec4 v1, vec4 v2);

float vector2_dot(vec2 *v1, vec2 *v2);
void vector2_normalize(vec2* v);
vec2 vector2_subtract(vec2 v1, vec2 v2);
vec2 vector2_add(vec2 v1, vec2 v2);
vec2 vector2_scale(vec2 v1, float s);
float vector2_distance(vec2 v1, vec2 v2);

void matrix_rotateY(mat4* m, float angle);
mat4 matrix_multiply(mat4* m1, mat4* m2);
mat4 matrix_lookAt(vec4 eye, vec4 center, vec4 up);
mat4 matrix_perspective(float fovy, float aspect_ratio, float near_plane, float far_plane);
mat4 matrix_ortho(float left, float right, float bottom, float top, float near_plane, float far_plane);

float radians(float dgr);
int FloatEquals(float a, float b, float floatEqualityThreshold);

#endif