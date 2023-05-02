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
#include <math.h>
#include "GL/glew.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>


#include "graphics.h"
#include "term.h"

#define TITLE "minimal game engine"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern int errorCode;
extern SDL_Event e;
extern int exitLoop;

extern float degrees;

extern unsigned long deltaTime;

extern SDL_RendererFlip flipType;

extern ParticleSystem *p1;



int init();
int initGL();
void quit();
int eventhandler();

extern SDL_Window *window;

extern SDL_GLContext *context;




#endif