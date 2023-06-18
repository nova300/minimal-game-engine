#include "graphics.h"
#include "engine.h"
#include "stdlib.h"
#include "shaders.h"

#define FBWIDTH 320
#define FBHEIGHT 200

GLuint program;
GLuint vao;
GLuint vbo;
GLuint texture;

vec4 colors[FBWIDTH * FBHEIGHT];

void fb_init()
{
    program = loadShaders(vertex_shader_2, fragment_shader_2);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    GLfloat vertices[] = 
    {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FBWIDTH, FBHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void fb_update()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, FBWIDTH, FBHEIGHT, GL_RGBA, GL_FLOAT, colors);
}

void fb_render()
{
    fb_update();
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(program, "colorTexture"), 0);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void fb_clear()
{
    for (int i = 0; i < FBWIDTH * FBHEIGHT; i++)
    {
        colors[i].x = 0.0f;
        colors[i].y = 0.0f;
        colors[i].z = 0.0f;
        colors[i].w = 0.0f;
    }
}

void fb_drawSineWave(int amplitude, float frequency, float of)
{
    float increment = 2.0f * 3.14f / FBWIDTH;
    int midPoint = FBHEIGHT / 2;
    int x, y;

    for (x = 0; x < FBWIDTH; x++) {
        y = (int)round(amplitude * sin(frequency * x * increment + of));
        y += midPoint;

        if (y >= 0 && y < FBHEIGHT) {
            int pixelIndex = (y * FBWIDTH + x);
            colors[pixelIndex].x = 1.0f;
            colors[pixelIndex].y = 0.0f;
            colors[pixelIndex].z = 0.0f;
            colors[pixelIndex].w = 1.0f;
        }
    }
    for (x = 0; x < FBWIDTH; x++) {
        y = (int)round(amplitude * sin(frequency * x * increment + of + 0.5f));
        y += midPoint;

        if (y >= 0 && y < FBHEIGHT) {
            int pixelIndex = (y * FBWIDTH + x);
            colors[pixelIndex].x = 0.0f;
            colors[pixelIndex].y = 1.0f;
            colors[pixelIndex].z = 0.0f;
            colors[pixelIndex].w = 1.0f;
        }
    }
    for (x = 0; x < FBWIDTH; x++) {
        y = (int)round(amplitude * sin(frequency * x * increment + of + 1.0f));
        y += midPoint;

        if (y >= 0 && y < FBHEIGHT) {
            int pixelIndex = (y * FBWIDTH + x);
            colors[pixelIndex].x = 0.0f;
            colors[pixelIndex].y = 0.0f;
            colors[pixelIndex].z = 1.0f;
            colors[pixelIndex].w = 1.0f;
        }
    }
}