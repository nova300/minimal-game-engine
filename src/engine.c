#include "engine.h"
#include "shaders.h"



double time = 0;
double deltaTime = 0;

GLFWwindow *window;

ParticleSystem *p1;

int exitLoop = 0;
float degrees = 0;

mat4 projectionMatrix;
mat4 viewMatrix;

vec3 c_pos = {{0, 0, 3}};
vec3 c_front = {{0, 0, -1}};
vec3 c_up = {{0, 1, 0}};

float fov = 60.0f;
int s_width = SCREEN_WIDTH;
int s_height = SCREEN_HEIGHT;

int main(void)
{
    printf("init: ");
    int err = init();
    if (err != 0)
    {
        err = err + 100;
        printf("error %i\n", err);
    }
    printf("ok\n");

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

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

    par_shapes__compute_welded_normals(tetrahedron);
    par_shapes__compute_welded_normals(octohedron);

    GeoObject *gobj = geo_obj_createFromParShape(mesh1);
    GeoObject *m1 = geo_obj_createFromParShape(tetrahedron);
    GeoObject *m2 = geo_obj_createFromParShape(octohedron);

    RenderQueue renderQueue1;
    rq_init(&renderQueue1, 10);

    rq_add_object(&renderQueue1, geo_obj_createFromParShape(octohedron));
    rq_add_object(&renderQueue1, geo_obj_createFromParShape(tetrahedron));

    Shader *s = newShaderObject(vertex_shader_0, fragment_shader_0);
    //gobj->shader = s;
    renderQueue1.gpuHandle.shader = s;

    projectionMatrix = matrix_perspective(radians(fov), (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);

    vec3 eye = {{5, 5, 5}};
    vec3 center = {{0, 0, 0}};
    vec3 up = {{0, 1, 0}};
    viewMatrix = matrix_lookAt(eye, center, up);

    GeoObject **rq = renderQueue1.objectBuffer;

    transform_set_identity(&gobj->baseTransform);
    transform_set_identity(&rq[0]->baseTransform);
    transform_set_identity(&rq[1]->baseTransform);

    renderQueue1.gpuHandle.textureAtlas = generateRandomAtlas();

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


    //p1 = particle_new(rq[1], 1024);
    fb_init();
    glBindVertexArray(VertexArrayID);
    while (exitLoop == 0)
    {
        key_input_poll();
        viewMatrix = matrix_lookAt(c_pos, vector_add(c_pos, c_front), c_up);
        projectionMatrix = matrix_perspective(radians(fov), (float)s_width/s_height, 0.1f, 100.0f);
        deltaTime = glfwGetTime() - time;
        if (deltaTime > 10) deltaTime = 10;
        time = glfwGetTime();

        transform_rotate(0, 0, 1 * deltaTime, &rq[1]->baseTransform);
        transform_rotate(1 * deltaTime, 0, 0, &rq[2]->baseTransform);

        //particle_update(p1);

        rq[2]->baseTexture = rand() % 50;
        rq[2]->instanceDirty = 1;

        glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        rq_update_buffers(&renderQueue1);
        
        geo_render(&renderQueue1.gpuHandle);

        fb_render();
        
        glBindVertexArray(VertexArrayID);

        glfwSwapBuffers(window);
        fb_clear();
        glfwPollEvents();
        if (glfwWindowShouldClose(window)) exitLoop = 1;
    }


    quit();
    printf("Goodbye.\n");
    return 0;
}

void helloworld()
{
    printf("hello world");
    return;
}