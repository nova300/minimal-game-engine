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


static const GLfloat g_vertex_buffer_data[] = {
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

static GLfloat g_color_buffer_data[12*3*3];


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
        printf("failed to open font\n");
    }

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);


    int prg = LoadShaders(vertex_shader_0, fragment_shader_0);

    GLuint ViewID = glGetUniformLocation(prg, "VIE");
    GLuint ModelID = glGetUniformLocation(prg, "MOD");
    GLuint ProjectionID = glGetUniformLocation(prg, "PRO");

    printf("view: %i model: %i proj: %i\n", ViewID, ModelID, ProjectionID);

    mat4 projmat = matrix_perspective(radians(45.0f), (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);

    vec4 eye = {{4, 3, 3, 0}};
    vec4 center = {{-3, -2, -2, 0}};
    vec4 up = {{0, 1, 0, 0}};
    mat4 view  = matrix_lookAt(eye, center, up);


    mat4 modelmat = IDENTITY_MATRIX;

    while (exitLoop == 0)
    {
        for (int v = 0; v < 12*3 ; v++)
        {
            g_color_buffer_data[3*v+0] = ((float)(rand() % 90))/ 100.f;
            g_color_buffer_data[3*v+1] = ((float)(rand() % 90)) / 100.f;
            g_color_buffer_data[3*v+2] = ((float)(rand() % 90)) / 100.f;
        }

        deltaTime = SDL_GetTicks() - time;
        eventhandler();
        if (deltaTime > 250) deltaTime = 250;
        time = SDL_GetTicks();

        glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        matrix_rotateY(&modelmat, 0.001 * deltaTime);

        glUseProgram(prg);

        glUniformMatrix4fv(ViewID, 1, GL_FALSE, &(view.m[0]));
        glUniformMatrix4fv(ModelID, 1, GL_FALSE, &(modelmat.m[0]));
        glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &(projmat.m[0]));

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);
        glDisableVertexAttribArray(0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);





        SDL_GL_SwapWindow( window );
    }


    quit();
    printf("Goodbye.\n");
    return 0;
}