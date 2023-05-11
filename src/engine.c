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

mat4 projectionMatrix;
mat4 viewMatrix;

ENTRYPOINT
{
    printf("init: ");
    terminal_print("init: ");
    int err = init();
    if (err != 0)
    {
        err = err + 100;
        printf("error %i\n", err);
    }
    printf("ok\n");
    terminal_print("ok\n");

    font = TTF_OpenFont("media/tosh.ttf", 28);
    if (font == NULL)
    {
        printf("TERM: failed to open font\n");
    }

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glGenBuffers(1, &transformBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, transformBuffer);

    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

    GeoObject *cube = geo_new_object();

    geo_obj_loadFromFile("media/sphere.obj", cube);

    cube->shader = newShaderObject(vertex_shader_1, fragment_shader_1);

    projectionMatrix = matrix_perspective(radians(45.0f), (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);

    vec3 eye = {{3, 3, 3}};
    vec3 center = {{0, 0, 0}};
    vec3 up = {{0, 1, 0}};
    viewMatrix = matrix_lookAt(eye, center, up);

    vec3 color1 = {{1.0f, 0.5f, 0.0f}};

    cube->color = color1;

    transform_set_identity(&cube->transform);


    if (loadTexture("media/textest.jpg", &cube->texture))
    {
        printf("could not load texture\n");
    }

    p1 = particle_new(cube, 100000);

    while (exitLoop == 0)
    {
        deltaTime = SDL_GetTicks() - time;
        eventhandler();
        if (deltaTime > 250) deltaTime = 250;
        time = SDL_GetTicks();

        transform_rotate(0.001 * deltaTime, 0.001 * deltaTime, 0.001 * deltaTime, &cube->transform);

        glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        geo_render(cube);
        //particle_render(p1);


        SDL_GL_SwapWindow( window );
    }


    quit();
    printf("Goodbye.\n");
    return 0;
}