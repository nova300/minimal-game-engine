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

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    GLuint uvBuffer;
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);

    triangleVertex[0].x = -1.0f;
    triangleVertex[0].y = -1.0f;
    triangleVertex[0].z =  0.0f;
    triangleVertex[1].x =  1.0f;
    triangleVertex[1].y = -1.0f;
    triangleVertex[1].z =  0.0f;
    triangleVertex[2].x =  0.0f;
    triangleVertex[2].y =  1.0f;
    triangleVertex[2].z =  0.0f;


    int prg = loadShaders(vertex_shader_0, fragment_shader_0);

    GLuint ViewID = glGetUniformLocation(prg, "VIE");
    GLuint ModelID = glGetUniformLocation(prg, "MOD");
    GLuint ProjectionID = glGetUniformLocation(prg, "PRO");

    GeoObject *cube = malloc(sizeof(GeoObject));
    GeoObject *triangle = malloc(sizeof(GeoObject));

    geo_obj_loadFromFile("media/cube.obj", cube);

    //cube->vertexBuffer = &cubeVertex;
    //cube->bufferLength = sizeof(cubeVertex);

    triangle->vertexBuffer = (vec3*)&triangleVertex;
    triangle->bufferLength = sizeof(triangleVertex);

    //cube->triCount = 12;
    triangle->triCount = 1;

    cube->PRO = matrix_perspective(radians(45.0f), (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);
    triangle->PRO = matrix_perspective(radians(90.0f), (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);

    vec4 eye = {{4, 3, 3, 0}};
    vec4 center = {{-3, -2, -2, 0}};
    vec4 up = {{0, 1, 0, 0}};
    cube->VIE = matrix_lookAt(eye, center, up);
    
    vec4 eye1 = {{2, 1, 1, 0}};
    vec4 center1 = {{0, 0, 0, 0}};
    vec4 up1 = {{0, 1, 0, 0}};

    triangle->VIE = matrix_lookAt(eye1, center1, up1);

    cube->MOD = IDENTITY_MATRIX;
    triangle->MOD = IDENTITY_MATRIX;

    if (loadTexture("media/font_0.png", &cube->texture))
    {
        printf("could not load texture\n");
    }

    while (exitLoop == 0)
    {
        deltaTime = SDL_GetTicks() - time;
        eventhandler();
        if (deltaTime > 250) deltaTime = 250;
        time = SDL_GetTicks();

        glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        matrix_rotateY(&cube->MOD, 0.001 * deltaTime);

        glUseProgram(prg);

        glBindTexture(GL_TEXTURE_2D, cube->texture);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glBufferData(GL_ARRAY_BUFFER, cube->bufferLength, (int*)cube->uvBuffer, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glUniformMatrix4fv(ViewID, 1, GL_FALSE, &(cube->VIE.m[0]));
        glUniformMatrix4fv(ModelID, 1, GL_FALSE, &(cube->MOD.m[0]));
        glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &(cube->PRO.m[0]));
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, cube->bufferLength, cube->vertexBuffer, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, cube->triCount*3);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);



        SDL_GL_SwapWindow( window );
    }


    quit();
    printf("Goodbye.\n");
    return 0;
}