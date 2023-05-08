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


GLfloat cubeVertex[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

vec3 triangleVertex[3]; 
/*= {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};*/

static GLfloat cube_color_buffer[12*3*3];

static GLfloat triangle_color_buffer[] = 
{
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

int vertexBuffer;
int uvBuffer;
int normalBuffer;
int elementBuffer;


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

    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);


    triangleVertex[0].x = -1.0f;
    triangleVertex[0].y = -1.0f;
    triangleVertex[0].z =  0.0f;
    triangleVertex[1].x =  1.0f;
    triangleVertex[1].y = -1.0f;
    triangleVertex[1].z =  0.0f;
    triangleVertex[2].x =  0.0f;
    triangleVertex[2].y =  1.0f;
    triangleVertex[2].z =  0.0f;

    GeoObject *cube = malloc(sizeof(GeoObject));
    GeoObject *triangle = malloc(sizeof(GeoObject));

    geo_obj_loadFromFile("media/sphere.obj", cube);

    cube->shader = newShaderObject(vertex_shader_0, fragment_shader_0);

    //cube->vertexBuffer = &cubeVertex;
    //cube->bufferLength = sizeof(cubeVertex);

    triangle->vertexBuffer = (vec3*)&triangleVertex;
    triangle->bufferLength = sizeof(triangleVertex);

    //cube->triCount = 12;
    triangle->triCount = 1;

    cube->PRO = matrix_perspective(radians(45.0f), (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);
    triangle->PRO = matrix_perspective(radians(90.0f), (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);

    vec3 eye = {{3, 3, 3}};
    vec3 center = {{0, 0, 0}};
    vec3 up = {{0, 1, 0}};
    cube->VIE = matrix_lookAt(eye, center, up);
    
    vec3 eye1 = {{2, 1, -1}};
    vec3 center1 = {{0, 0, 0}};
    vec3 up1 = {{0, 1, 0}};

    vec3 color1 = {{1.0f, 0.5f, 0.0f}};

    triangle->VIE = matrix_lookAt(eye1, center1, up1);

    cube->MOD = IDENTITY_MATRIX;
    triangle->MOD = IDENTITY_MATRIX;

    if (loadTexture("media/textest.jpg", &cube->texture))
    {
        printf("could not load texture\n");
    }

    float colorTimer = 0;

    while (exitLoop == 0)
    {
        deltaTime = SDL_GetTicks() - time;
        eventhandler();
        if (deltaTime > 250) deltaTime = 250;
        time = SDL_GetTicks();

        if (colorTimer < 0)
        {
            color1.x = ((float)(rand() % 100)/100.0f);
            color1.y = ((float)(rand() % 100)/100.0f);
            color1.z = ((float)(rand() % 100)/100.0f);
            colorTimer = 400.0f;
        }
        else
        {
            colorTimer -= deltaTime;
        }

        glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        matrix_rotateY(&cube->MOD, 0.001 * deltaTime);

        
        geo_render(cube);


        SDL_GL_SwapWindow( window );
    }


    quit();
    printf("Goodbye.\n");
    return 0;
}