#ifndef engine_h
#define engine_h

#define GLEW_STATIC
#define DO_MMX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "GL/glew.h"
#include <GL/glu.h>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "graphics.h"

#define TITLE "minimal game engine"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

typedef unsigned int (*functionPointer)();
typedef unsigned int (*functionPointerFloat)(float);

typedef struct
{
    functionPointer init;
    functionPointerFloat update;
    functionPointer destroy;
}Program;

extern Program **programStack;
extern int programCapacity;
extern int programTop;

extern int errorCode;
extern int exitLoop;

extern float degrees;
extern double deltaTime;
extern double time;
extern ParticleSystem *p1;

extern mat4 viewMatrix;
extern mat4 projectionMatrix;

extern vec4 c_pos;
extern vec4 c_front;
extern vec4 c_up;

extern float fov;
extern int s_width;
extern int s_height;



int init();
int initGL();
void quit();

int program_init(void);
int program_free(void);
int program_push(Program *program);
int program_pop(void);
int program_update(float deltaTime);
Program *program_top(void);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void error_callback(int error, const char* description);
void key_input_poll(void);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

extern GLFWwindow *window;




#endif