#ifndef engine_h
#define engine_h


#ifdef _WIN32
#define ENTRYPOINT int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
#define ENTRYPOINT int main(int argc, char **argv)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
#include <GL/GLU.h>

#include "graphics.h"
#include "term.h"

#define TITLE "minimal game engine"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

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