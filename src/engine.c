#include "engine.h"
#include "shaders.h"

int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 768;

unsigned long time = 0;
unsigned long deltaTime = 0;

SDL_Window *window;
SDL_GLContext *context;

TTF_Font *font;

ParticleSystem *p1;

int exitLoop = 0;
float degrees = 0;

int vertexBuffer;
int transformBuffer;
int elementBuffer;
int colorBuffer;
int indirectBuffer;
int textureBuffer;

mat4 projectionMatrix;
mat4 viewMatrix;

ENTRYPOINT
{
    printf("init: ");
    //terminal_print("init: ");
    int err = init();
    if (err != 0)
    {
        err = err + 100;
        printf("error %i\n", err);
    }
    printf("ok\n");
    //terminal_print("ok\n");

    /*font = TTF_OpenFont("media/tosh.ttf", 28);
    if (font == NULL)
    {
        printf("TERM: failed to open font\n");
    }*/

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glGenBuffers(1, &transformBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, transformBuffer);

    glGenBuffers(1, &textureBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);

    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

    glGenBuffers(1, &indirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);


    //GeoObject *cube = geo_new_object();

    //geo_obj_loadFromFile("media/scene.obj", cube);


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

    GeoObject *rq[3];

    rq[0] = geo_obj_createFromParShape(octohedron);
    rq[1] = geo_obj_createFromParShape(tetrahedron);

    Shader *s = newShaderObject(vertex_shader_0, fragment_shader_0);
    gobj->shader = s;
    rq[0]->shader = s;
    rq[1]->shader = s;

    projectionMatrix = matrix_perspective(radians(45.0f), (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);

    vec3 eye = {{50, 50, 50}};
    vec3 center = {{0, 0, 0}};
    vec3 up = {{0, 1, 0}};
    viewMatrix = matrix_lookAt(eye, center, up);

    transform_set_identity(&gobj->baseTransform);
    transform_set_identity(&rq[0]->baseTransform);
    transform_set_identity(&rq[1]->baseTransform);

    /*if (loadTexture("media/textest.jpg", &gobj->texture))
    {
        printf("could not load texture\n");
    }*/

    int atlas = generateRandomAtlas();

    gobj->baseTexture = 5;
    rq[0]->baseTexture = 2;
    rq[1]->baseTexture = 8;
    rq[2] = gobj;

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


    p1 = particle_new(gobj, 512);

    while (exitLoop == 0)
    {
        glViewport(0, 0, SDL_GetWindowSurface(window)->w, SDL_GetWindowSurface(window)->h);
        deltaTime = SDL_GetTicks() - time;
        eventhandler();
        if (deltaTime > 250) deltaTime = 250;
        time = SDL_GetTicks();

        transform_rotate(0, 0, 0.001 * deltaTime, &rq[1]->baseTransform);
        //transform_rotate(0.001 * deltaTime, 0, 0, &rq[2]->baseTransform);

        particle_update(p1);

        //rq[2]->baseTexture = rand() % 50;

        glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        geo_render_multi(rq, 3, atlas, s);


        SDL_GL_SwapWindow( window );
    }


    quit();
    printf("Goodbye.\n");
    return 0;
}