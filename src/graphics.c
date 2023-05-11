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

/* Object Transforms */

int transform_position(float x, float y, float z, Transform *t)
{
    t->position.x = x;
    t->position.y = y;
    t->position.z = z;
    transform_make_matrix(t);
}

int transform_move(float x, float y, float z, Transform *t)
{
    t->position.x += x;
    t->position.y += y;
    t->position.z += z;
    transform_make_matrix(t);
}

int transform_rotate(float x, float y, float z, Transform *t)
{
    t->rotation.x += x;
    t->rotation.y += y;
    t->rotation.z += z;
    transform_make_matrix(t);
}

int transform_set_rotation(float x, float y, float z, Transform *t)
{
    t->rotation.x = x;
    t->rotation.y = y;
    t->rotation.z = z;
    transform_make_matrix(t);
}

int transform_make_matrix(Transform *t) {

    mat4 model_matrix;
    memset(&model_matrix, 0, sizeof(mat4));

    // Translation matrix
    model_matrix.w.x = t->position.x;
    model_matrix.w.y = t->position.y;
    model_matrix.w.z = t->position.z;
    model_matrix.w.w = 1.0f;

    // Rotation matrix
    float cx = cosf(t->rotation.x);
    float sx = sinf(t->rotation.x);
    float cy = cosf(t->rotation.y);
    float sy = sinf(t->rotation.y);
    float cz = cosf(t->rotation.z);
    float sz = sinf(t->rotation.z);

    model_matrix.x.x = cy * cz;
    model_matrix.x.y = -cy * sz;
    model_matrix.x.z = sy;
    model_matrix.y.x = cz * sx * sy + cx * sz;
    model_matrix.y.y = cx * cz - sx * sy * sz;
    model_matrix.y.z = -cy * sx;
    model_matrix.z.x = -cx * cz * sy + sx * sz;
    model_matrix.z.y = cz * sx + cx * sy * sz;
    model_matrix.z.z = cx * cy;
    model_matrix.w.w = 1.0f;

    // Scale matrix
    model_matrix.x.x *= t->scale.x;
    model_matrix.x.y *= t->scale.x;
    model_matrix.x.z *= t->scale.x;
    model_matrix.y.x *= t->scale.y;
    model_matrix.y.y *= t->scale.y;
    model_matrix.y.z *= t->scale.y;
    model_matrix.z.x *= t->scale.z;
    model_matrix.z.y *= t->scale.z;
    model_matrix.z.z *= t->scale.z;

    t->matrix = model_matrix;
}

int transform_set_identity(Transform* t) 
{
    t->position = (vec3){0.0f, 0.0f, 0.0f};
    t->rotation = (vec3){0.0f, 0.0f, 0.0f};
    t->scale = (vec3){1.0f, 1.0f, 1.0f};
    t->matrix = IDENTITY_MATRIX;
}


/* Graphics Objects */

GeoObject *geo_new_object()
{
    GeoObject *g = malloc(sizeof(GeoObject));
    g->data = NULL;
    g->indicies = NULL;
    g->shader = NULL;

    geo_init_transformArray(&g->transformArray, 1);

    return g;
}

int geo_render(GeoObject *obj)
{
    glUseProgram(obj->shader->ShaderID);
    glBindTexture(GL_TEXTURE_2D, obj->texture);

    glUniform3fv(obj->shader->ColorID, 1, &(obj->color.m[0]));
    //glUniformMatrix4fv(obj->shader->ModelID, 1, GL_FALSE, &(obj->transform.matrix.m[0]));
    glUniformMatrix4fv(obj->shader->ViewID, 1, GL_FALSE, &(viewMatrix.m[0]));
    glUniformMatrix4fv(obj->shader->ProjectionID, 1, GL_FALSE, &(projectionMatrix.m[0]));
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, obj->dataCount * sizeof(vertex), obj->data, GL_STATIC_DRAW);



    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->indexCount * sizeof(unsigned int), obj->indicies, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, transformBuffer);
    glBufferData(GL_ARRAY_BUFFER, obj->transformArray.count * sizeof(mat4), obj->transformArray.data, GL_DYNAMIC_DRAW);

    int model_matrix_location = 3;
    glEnableVertexAttribArray(model_matrix_location);
    glEnableVertexAttribArray(model_matrix_location + 1);
    glEnableVertexAttribArray(model_matrix_location + 2);
    glEnableVertexAttribArray(model_matrix_location + 3);
    glVertexAttribPointer(model_matrix_location, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)0);
    glVertexAttribPointer(model_matrix_location + 1, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4)));
    glVertexAttribPointer(model_matrix_location + 2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(2*sizeof(vec4)));
    glVertexAttribPointer(model_matrix_location + 3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(3*sizeof(vec4)));
    glVertexAttribDivisor(model_matrix_location, 1);
    glVertexAttribDivisor(model_matrix_location + 1, 1);
    glVertexAttribDivisor(model_matrix_location + 2, 1);
    glVertexAttribDivisor(model_matrix_location + 3, 1);


    glDrawElementsInstanced(GL_TRIANGLES, obj->indexCount, GL_UNSIGNED_INT, (void*)0, obj->transformArray.count);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(model_matrix_location + 1);
    glDisableVertexAttribArray(model_matrix_location + 2);
    glDisableVertexAttribArray(model_matrix_location + 3);
}

int geo_render_translated(GeoObject *obj, Transform *t)
{
    glUseProgram(obj->shader->ShaderID);
    glBindTexture(GL_TEXTURE_2D, obj->texture);

    glUniform3fv(obj->shader->ColorID, 1, &(obj->color.m[0]));
    glUniformMatrix4fv(obj->shader->ModelID, 1, GL_FALSE, &(t->matrix.m[0]));
    glUniformMatrix4fv(obj->shader->ViewID, 1, GL_FALSE, &(viewMatrix.m[0]));
    glUniformMatrix4fv(obj->shader->ProjectionID, 1, GL_FALSE, &(projectionMatrix.m[0]));
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, obj->dataCount * sizeof(vertex), obj->data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->indexCount * sizeof(unsigned int), obj->indicies, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coordinates


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glDrawElements(GL_TRIANGLES, obj->indexCount, GL_UNSIGNED_INT, (void*)0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
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


int particle_render(ParticleSystem *ps)
{
    geo_clear_transformArray(&ps->geo->transformArray);
    for (int i = 0; i < ps->amount; i++)
    {
        if (ps->particles[i].lifeTime < 0)
        {
            
            ps->particles[i].lifeTime = (rand() % 500) + 250;
            ps->particles[i].transform = ps->transform;
            ps->particles[i].xdir = ((rand() - rand()) % 3) + ((rand() - rand()) % 10) * 0.1f;
            ps->particles[i].ydir = ((rand() - rand()) % 3) + ((rand() - rand()) % 10) * 0.1f;
            ps->particles[i].zdir = ((rand() - rand()) % 3) + ((rand() - rand()) % 10) * 0.1f;
        }
        else
        {
            ps->particles[i].lifeTime = ps->particles[i].lifeTime - (deltaTime);
            transform_move(ps->particles[i].xdir * (deltaTime*0.01), ps->particles[i].ydir * (deltaTime*0.01), ps->particles[i].zdir * (deltaTime*0.01), &ps->particles[i].transform);
            geo_add_transformArray(&ps->geo->transformArray, ps->particles[i].transform.matrix);
        }
    }
    geo_render(ps->geo);
}

int particle_render_colorful(ParticleSystem *ps)
{
    for (int i = 0; i < ps->amount; i++)
    {
        if (ps->particles[i].lifeTime < 0)
        {
            
            ps->particles[i].lifeTime = (rand() % 500) + 250;
            ps->particles[i].transform = ps->transform;
            ps->particles[i].xdir = ((rand() - rand()) % 3) + ((rand() - rand()) % 10) * 0.1f;
            ps->particles[i].ydir = ((rand() - rand()) % 3) + ((rand() - rand()) % 10) * 0.1f;
            ps->particles[i].zdir = ((rand() - rand()) % 3) + ((rand() - rand()) % 10) * 0.1f;
            ps->particles[i].color.x = ((float)(rand() % 100)/100.0f);
            ps->particles[i].color.y = ((float)(rand() % 100)/100.0f);
            ps->particles[i].color.z = ((float)(rand() % 100)/100.0f);
        }
        else
        {
            ps->particles[i].lifeTime = ps->particles[i].lifeTime - (deltaTime);
            transform_move(ps->particles[i].xdir * (deltaTime*0.01), ps->particles[i].ydir * (deltaTime*0.01), ps->particles[i].zdir * (deltaTime*0.01), &ps->particles[i].transform);
            ps->geo->color = ps->particles[i].color;
            geo_render_translated(ps->geo, &ps->particles[i].transform);
        }
    }
}

ParticleSystem* particle_new(GeoObject *g, int amount)
{
    ParticleSystem *ps = malloc(sizeof(ParticleSystem));
    Transform *t = (Transform*)ps;
    Particle *p = malloc(sizeof(Particle) * amount);
    ps->geo = g;
    ps->amount = amount;

    transform_set_identity(t);

    
    for (int i = 0; i < amount; i++)
    {
        Transform *pt = (Transform*)&p[i];
        p[i].lifeTime = 0;
        p[i].xdir = 0;
        p[i].ydir = 0;
        p[i].zdir = 0;
        pt->position.x = 0;
        pt->position.y = 0;
        pt->position.z = 0;
    }
    
    ps->particles = p;

    return ps;
}


