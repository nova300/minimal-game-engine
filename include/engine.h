#ifndef engine_h
#define engine_h

#define GLEW_STATIC

#ifdef _WIN32
#define ENTRYPOINT int WINAPI WinMain (int argc, char **argv)
#else
#define ENTRYPOINT int main(int argc, char **argv)
#endif

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

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern int errorCode;
extern int exitLoop;

extern float degrees;
extern double deltaTime;
extern ParticleSystem *p1;

extern mat4 viewMatrix;
extern mat4 projectionMatrix;



int init();
int initGL();
void quit();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void error_callback(int error, const char* description);

extern GLFWwindow *window;




#endif