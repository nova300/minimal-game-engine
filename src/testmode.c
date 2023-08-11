#include "engine.h"
#include "shaders.h"

GeoObject **rq;
RenderQueue renderQueue1;
RenderQueue renderQueue2;
Program *this;

int testprogram_init()
{
    par_shapes_mesh *dodecahedron = par_shapes_create_dodecahedron();
    par_shapes_translate(dodecahedron, 0, 0.934, 0);

    par_shapes_mesh *tetrahedron = par_shapes_create_icosahedron();
    par_shapes_translate(tetrahedron, 1, 0, 3.5);

    par_shapes_mesh *octohedron = par_shapes_create_octahedron();
    par_shapes_translate(octohedron, -2.25, 0.9, -.5);

    par_shapes_mesh *icosahedron = par_shapes_create_icosahedron();
    par_shapes_translate(icosahedron, -1, 0.8, 3.5);

    par_shapes_mesh *cube = par_shapes_create_cube();
    par_shapes_rotate(cube, PAR_PI / 5.0, (float[]){0, 1, 0});
    par_shapes_translate(cube, 1, 0, 0.5);
    par_shapes_scale(cube, 1.2, 1.2, 1.2);

    par_shapes_mesh *mesh1 = par_shapes_create_empty();
    par_shapes_merge_and_free(mesh1, dodecahedron);
    par_shapes_merge_and_free(mesh1, icosahedron);
    par_shapes_merge_and_free(mesh1, cube);
    par_shapes__compute_welded_normals(mesh1);

    par_shapes_compute_normals(tetrahedron);
    par_shapes_compute_normals(octohedron);

    GeoObject *gobj = geo_obj_createFromParShape(mesh1);
    GeoObject *m1 = geo_obj_createFromParShape(tetrahedron);
    GeoObject *m2 = geo_obj_createFromParShape(octohedron);

    rq_init(&renderQueue1, 10);

    rq_init(&renderQueue2, 10);

    rq_add_object(&renderQueue2, geo_obj_createFromParShape(tetrahedron));

    rq_add_object(&renderQueue1, geo_obj_createFromParShape(octohedron));
    rq_add_object(&renderQueue1, geo_obj_createFromParShape(tetrahedron));

    Shader *s = newShaderObject(vertex_shader_0, fragment_shader_0);
    //gobj->shader = s;
    renderQueue1.gpuHandle.shader = s;

    s = newShaderObject(vertex_shader_2, fragment_shader_2);
    renderQueue2.gpuHandle.shader = s;

    rq = renderQueue1.objectBuffer;

    transform_set_identity(&gobj->baseTransform);
    transform_set_identity(&rq[0]->baseTransform);
    transform_set_identity(&rq[1]->baseTransform);

    renderQueue1.gpuHandle.textureAtlas = generateRandomAtlas();
    renderQueue2.gpuHandle.textureAtlas = generateRandomAtlas();

    gobj->baseTexture = 5;
    rq[0]->baseTexture = 2;
    rq[1]->baseTexture = 8;
    rq_add_object(&renderQueue1, gobj);

    geo_instanceop_init(rq[0], 10);
    Transform ta;
    Transform tb;
    Transform tc;
    transform_set_identity(&ta);
    transform_set_identity(&tb);
    transform_set_identity(&tc);
    transform_move(0, -1, 0, &ta);
    transform_move(0, 1, 0, &tb);
    transform_move(1, 0, 0, &tc);
    geo_instanceop_add(rq[0], ta.matrix, 4);
    geo_instanceop_add(rq[0], tb.matrix, 16);
    geo_instanceop_add(rq[0], tc.matrix, 16);


    par_shapes_mesh *shape = par_shapes_create_plane(3, 3);
    par_shapes_translate(shape, -0.5, 0, 1);
    par_shapes_scale(shape, 4, 1.5, 1);

    rq_add_object(&renderQueue1, geo_obj_createFromParShape(shape));

    GeoObject *cube1 = geo_new_object();
    geo_obj_loadFromFile("media/cube.obj", cube1);

    rq_add_object(&renderQueue1, cube1);
}

int testprogram_update(float deltaTime)
{
    transform_rotate(0, 0, 1 * deltaTime, &rq[1]->baseTransform);
    transform_rotate(1 * deltaTime, 0, 0, &rq[2]->baseTransform);

    // particle_update(p1);

    rq[2]->baseTexture = rand() % 50;
    rq[2]->instanceDirty = 1;

    rq_update_buffers(&renderQueue1);
    rq_update_buffers(&renderQueue2);

    geo_render(&renderQueue1.gpuHandle);
    geo_render(&renderQueue2.gpuHandle);
}

int testprogram_destroy()
{
    free(this);
}

Program *program_get_testmode()
{
    this = malloc(sizeof(Program));

    this->init = testprogram_init;
    this->update = testprogram_update;
    this->destroy = testprogram_destroy;
}