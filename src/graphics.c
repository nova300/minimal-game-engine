#include "graphics.h"
#include "engine.h"

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
    {
      	for (column = 0; column < 4; ++column)
        {
            out.m[row_offset + column] = (m1->m[row_offset + 0] * m2->m[column + 0]) + (m1->m[row_offset + 1] * m2->m[column + 4]) + (m1->m[row_offset + 2] * m2->m[column + 8]) + (m1->m[row_offset + 3] * m2->m[column + 12]);  
        }
    }

	return out;
}

/* Object Transforms */

void transform_position(float x, float y, float z, Transform *t)
{
    t->position.x = x;
    t->position.y = y;
    t->position.z = z;
    transform_make_matrix(t);
}

void transform_move(float x, float y, float z, Transform *t)
{
    t->position.x += x;
    t->position.y += y;
    t->position.z += z;
    transform_make_matrix(t);
}

void transform_rotate(float x, float y, float z, Transform *t)
{
    // Convert Euler angles to quaternion
    float cy = cosf(z * 0.5f);
    float sy = sinf(z * 0.5f);
    float cr = cosf(x * 0.5f);
    float sr = sinf(x * 0.5f);
    float cp = cosf(x * 0.5f);
    float sp = sinf(x * 0.5f);

    vec4 rotation;
    rotation.x = sr * cp * cy - cr * sp * sy;
    rotation.y = cr * sp * cy + sr * cp * sy;
    rotation.z = cr * cp * sy - sr * sp * cy;
    rotation.w = cr * cp * cy + sr * sp * sy;

    vec4 rotated_q;
    rotated_q.w = t->rotation.w * rotation.w - t->rotation.x * rotation.x - t->rotation.y * rotation.y - t->rotation.z * rotation.z;
    rotated_q.x = t->rotation.w * rotation.x + t->rotation.x * rotation.w + t->rotation.y * rotation.z - t->rotation.z * rotation.y;
    rotated_q.y = t->rotation.w * rotation.y - t->rotation.x * rotation.z + t->rotation.y * rotation.w + t->rotation.z * rotation.x;
    rotated_q.z = t->rotation.w * rotation.z + t->rotation.x * rotation.y - t->rotation.y * rotation.x + t->rotation.z * rotation.w;

    t->rotation = rotated_q;

    transform_make_matrix(t);
}

void transform_set_rotation(float x, float y, float z, Transform *t)
{
    float cy = cosf(z * 0.5f);
    float sy = sinf(z * 0.5f);
    float cr = cosf(x * 0.5f);
    float sr = sinf(x * 0.5f);
    float cp = cosf(y * 0.5f);
    float sp = sinf(y * 0.5f);

    t->rotation.x = sr * cp * cy - cr * sp * sy;
    t->rotation.y = cr * sp * cy + sr * cp * sy;
    t->rotation.z = cr * cp * sy - sr * sp * cy;
    t->rotation.w = cr * cp * cy + sr * sp * sy;
    transform_make_matrix(t);
}

void transform_make_matrix(Transform *t) 
{

    mat4 model_matrix;
    memset(&model_matrix, 0, sizeof(mat4));

    // Translation matrix
    model_matrix.w.x = t->position.x;
    model_matrix.w.y = t->position.y;
    model_matrix.w.z = t->position.z;
    model_matrix.w.w = 1.0f;

    // Rotation matrix
    float x = t->rotation.x;
    float y = t->rotation.y;
    float z = t->rotation.z;
    float w = t->rotation.w;

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float ww = w * w;

    model_matrix.x.x = xx - yy - zz + ww;
    model_matrix.y.x = 2.0f * (x * y + z * w);
    model_matrix.z.x = 2.0f * (x * z - y * w);

    model_matrix.x.y = 2.0f * (x * y - z * w);
    model_matrix.y.y = -xx + yy - zz + ww;
    model_matrix.z.y = 2.0f * (y * z + x * w);

    model_matrix.x.z = 2.0f * (x * z + y * w);
    model_matrix.y.z = 2.0f * (y * z - x * w);
    model_matrix.z.z = -xx - yy + zz + ww;

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

void transform_set_identity(Transform* t) 
{
    t->position = (vec4){0.0f, 0.0f, 0.0f, 0.0f};
    t->rotation = (vec4){0.0f, 0.0f, 0.0f, 1.0f};
    t->scale = (vec4){1.0f, 1.0f, 1.0f, 0.0f};
    t->matrix = IDENTITY_MATRIX;
}

/*void geo_render(GeoObject_gpu *gobj)
{
    GeoObject *obj = (GeoObject*)gobj;
    glUseProgram(gobj->shader->ShaderID);
    glUniformMatrix4fv(gobj->shader->ViewID, 1, GL_FALSE, &(viewMatrix.m[0]));
    glUniformMatrix4fv(gobj->shader->ProjectionID, 1, GL_FALSE, &(projectionMatrix.m[0]));
    glBindTexture(GL_TEXTURE_2D, obj->baseTexture);
    
    glBindBuffer(GL_ARRAY_BUFFER, gobj->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, obj->dataCount * sizeof(vertex), obj->data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gobj->elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->indexCount * sizeof(unsigned int), obj->indicies, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, gobj->textureBuffer);
    glBufferData(GL_ARRAY_BUFFER, obj->instanceCount * sizeof(unsigned int), obj->texture, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1); 

    glBindBuffer(GL_ARRAY_BUFFER, gobj->transformBuffer);
    glBufferData(GL_ARRAY_BUFFER, obj->instanceCount * sizeof(mat4), obj->transform, GL_DYNAMIC_DRAW);
    int model_matrix_location = 4;
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

    glDrawElementsInstanced(GL_TRIANGLES, obj->indexCount, GL_UNSIGNED_INT, (void*)0, obj->instanceCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(model_matrix_location);
    glDisableVertexAttribArray(model_matrix_location + 1);
    glDisableVertexAttribArray(model_matrix_location + 2);
    glDisableVertexAttribArray(model_matrix_location + 3);
}*/

void geo_render(GeoObject_gpu_handle *rq)
{
    glUseProgram(rq->shader->ShaderID);
    glUniformMatrix4fv(rq->shader->ViewID, 1, GL_FALSE, &(viewMatrix.m[0]));
    glUniformMatrix4fv(rq->shader->ProjectionID, 1, GL_FALSE, &(projectionMatrix.m[0]));

    glBindTexture(GL_TEXTURE_2D_ARRAY, rq->textureAtlas);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rq->elementBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, rq->vertexBuffer);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coordinates

    glBindBuffer(GL_ARRAY_BUFFER, rq->textureBuffer);
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, 0, (void*)0);
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ARRAY_BUFFER, rq->transformBuffer);
    int model_matrix_location = 4;
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

    switch (rq->type)
    {
    case GOBJ_TYPE_MULTI:
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, rq->commandBuffer);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, rq->count, 0);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        break;

    case GOBJ_TYPE_SINGLE:
        glDrawElementsInstanced(GL_TRIANGLES, rq->indexCount, GL_UNSIGNED_INT, (void*)0, rq->count);
        break;
    
    default:
        printf("GRAPHICS: could not determine handle render type, skipping draw call\n");
        break;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(model_matrix_location);
    glDisableVertexAttribArray(model_matrix_location + 1);
    glDisableVertexAttribArray(model_matrix_location + 2);
    glDisableVertexAttribArray(model_matrix_location + 3);
}

/*void geo_render_translated(GeoObject_gpu *gobj, Transform *t)
{
    GeoObject *obj = (GeoObject*)gobj;

    glUseProgram(gobj->shader->ShaderID);
    glUniformMatrix4fv(gobj->shader->ViewID, 1, GL_FALSE, &(viewMatrix.m[0]));
    glUniformMatrix4fv(gobj->shader->ProjectionID, 1, GL_FALSE, &(projectionMatrix.m[0]));

    glBindTexture(GL_TEXTURE_2D, obj->baseTexture);
    
    glBindBuffer(GL_ARRAY_BUFFER, gobj->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, obj->dataCount * sizeof(vertex), obj->data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gobj->elementBuffer);
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
}*/

GLuint generateColorTexture(float r, float g, float b, float a)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLfloat colors[4];

    colors[0] = r;
    colors[1] = g;
    colors[2] = b;
    colors[3] = a;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, colors);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return texture;
}

//generates array of 100 random color textures
GLuint generateRandomAtlas(void)
{
    GLuint texture;
    glGenTextures(1, &texture);
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 1, 1, 50);

    for (unsigned int i = 0; i < 50; ++i)
    {
        GLubyte colors[4];
        colors[0] = ((GLubyte)(rand() % 255));
        colors[1] = ((GLubyte)(rand() % 255));
        colors[2] = ((GLubyte)(rand() % 255));
        colors[3] = 255;

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, colors);
    }

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return texture;
}

GLuint loadTexture(const char *name)
{
    /*SDL_Surface *s = IMG_Load(name);
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

    *texture = textureID;*/
    return 0;
}

void rq_update_buffers(RenderQueue *rq)
{
    char dataNeedsUpdate = 0;
    char instanceNeedUpdate = 0;
    char commandsNeedUpdate = 0;
    int count = rq->count;
    GeoObject **obj = rq->objectBuffer;
    for (int i = 0; i < count; i++)
    {
        dataNeedsUpdate = dataNeedsUpdate | obj[i]->dataDirty;
        obj[i]->dataDirty = 0;
        instanceNeedUpdate = instanceNeedUpdate | obj[i]->instanceDirty;
        obj[i]->instanceDirty = 0;
    }

    if (dataNeedsUpdate)
    {
        int indexCount = 0;
        int vertexCount = 0;
        for (int i = 0; i < count; i++)
        {
            indexCount += obj[i]->indexCount;
            vertexCount += obj[i]->dataCount;
        }

        int *indicies = malloc(indexCount * sizeof(int));
        vertex *vertices = malloc(vertexCount * sizeof(vertex));
        int vertex_offset = 0;
        int index_offset = 0;
        for (int i = 0; i < count; i++) 
        {
            memcpy(vertices + vertex_offset, obj[i]->data, obj[i]->dataCount * sizeof(vertex));
            memcpy(indicies + index_offset, obj[i]->indicies, obj[i]->indexCount * sizeof(int));
            vertex_offset += obj[i]->dataCount;
            index_offset += obj[i]->indexCount;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rq->gpuHandle.elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indicies, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, rq->gpuHandle.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(vertex), vertices, GL_STATIC_DRAW);

        commandsNeedUpdate = 1;

        free(indicies);
        free(vertices);
    }

    if (instanceNeedUpdate)
    {
        int instanceCount = 0;
        for (int i = 0; i < count; i++)
        {
            instanceCount += obj[i]->instanceCount;
        }
        int *textures = malloc(sizeof(int) * instanceCount);
        mat4 *transforms = malloc(sizeof(mat4) * instanceCount);
        int instance_offset = 0;
        for (int i = 0; i < count; i++)
        {
            memcpy(textures + instance_offset, obj[i]->texture, obj[i]->instanceCount * sizeof(int));
            memcpy(transforms + instance_offset, obj[i]->transform, obj[i]->instanceCount * sizeof(mat4));
            instance_offset += obj[i]->instanceCount;
        }

        glBindBuffer(GL_ARRAY_BUFFER, rq->gpuHandle.textureBuffer);
        glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(int), textures, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, rq->gpuHandle.transformBuffer);
        glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(mat4), transforms, GL_DYNAMIC_DRAW);

        commandsNeedUpdate = 1;

        free(textures);
        free(transforms);
    }

    if (commandsNeedUpdate)
    {
        drawCommand *commands = malloc(sizeof(drawCommand) * count);
        int baseVert = 0;
        int fIndex = 0;
        int baseInst = 0;
        for (int i = 0; i < count; i++)
        {
            commands[i].count = obj[i]->indexCount;
            commands[i].instanceCount = obj[i]->instanceCount;
            commands[i].firstIndex = fIndex;
            commands[i].baseVertex = baseVert;
            commands[i].baseInstance = baseInst;
            baseVert += obj[i]->dataCount;
            fIndex += obj[i]->indexCount;
            baseInst += obj[i]->instanceCount;
        }

        if (DO_INTEL_WORKAROUND)
        {
            glDeleteBuffers(1, &rq->gpuHandle.commandBuffer);
            glGenBuffers(1, &rq->gpuHandle.commandBuffer);    //hacky workaround for misbehaving intel gpus
        }

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, rq->gpuHandle.commandBuffer);
        if (rq->gpuHandle.count == count && !DO_INTEL_WORKAROUND)
        {
            glBufferSubData(GL_DRAW_INDIRECT_BUFFER, (void*)0, sizeof(drawCommand) * count, commands);
        }
        else
        {
            glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(drawCommand) * count, commands, GL_DYNAMIC_DRAW);
        }
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        

        free(commands);

        rq->gpuHandle.count = count;
    }
}

void geo_obj_gpu_update_buffers(GeoObject_gpu *gobj)
{

    if (gobj->geoObject.dataDirty)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gobj->gpuHandle.elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, gobj->geoObject.indexCount * sizeof(unsigned int), gobj->geoObject.indicies, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, gobj->gpuHandle.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, gobj->geoObject.dataCount * sizeof(vertex), gobj->geoObject.data, GL_STATIC_DRAW);

    }

    if (gobj->geoObject.instanceDirty)
    {
        glBindBuffer(GL_ARRAY_BUFFER, gobj->gpuHandle.textureBuffer);
        glBufferData(GL_ARRAY_BUFFER, gobj->geoObject.instanceCount * sizeof(int), gobj->geoObject.texture, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, gobj->gpuHandle.transformBuffer);
        glBufferData(GL_ARRAY_BUFFER, gobj->geoObject.instanceCount * sizeof(mat4), gobj->geoObject.transform, GL_STATIC_DRAW);
    }

}

RenderQueue *rq_new_queue(int capacity)
{
    RenderQueue *rq = malloc(sizeof(RenderQueue));
    rq_init(rq, capacity);
}

void rq_init(RenderQueue *rq, int capacity)
{
    rq->capacity = capacity;
    GeoObject **buf;
    buf = malloc(sizeof(*buf) * capacity);
    rq->objectBuffer = buf;
    rq->count = 0;

    geo_obj_gpu_handle_genBuffers(&rq->gpuHandle, GOBJ_TYPE_MULTI);
}

void geo_obj_gpu_handle_genBuffers(GeoObject_gpu_handle *gpuHandle, unsigned int type)
{
    glGenBuffers(1, &gpuHandle->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gpuHandle->vertexBuffer);

    glGenBuffers(1, &gpuHandle->transformBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gpuHandle->transformBuffer);

    glGenBuffers(1, &gpuHandle->textureBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gpuHandle->textureBuffer);

    glGenBuffers(1, &gpuHandle->elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuHandle->elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    switch (type)
    {
    case GOBJ_TYPE_MULTI:
        glGenBuffers(1, &gpuHandle->commandBuffer);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, gpuHandle->commandBuffer);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        gpuHandle->type = type;
        break;
    
    default:
        gpuHandle->type = type;
        break;
    }
}

void rq_add_object(RenderQueue *rq, GeoObject *obj)
{
    if (rq->capacity <= rq->count)
    {
        printf("could not add object to renderqueue, at capacity\n");
        return;
    }

    obj->dataDirty = 1;
    obj->instanceDirty = 1;

    GeoObject **buf = rq->objectBuffer;

    buf[rq->count] = obj;
    rq->count++;
}




