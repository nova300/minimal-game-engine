#ifndef engine_h
#define engine_h


#ifdef _WIN32
#define ENTRYPOINT WinMain
#else
#define ENTRYPOINT main
#endif

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "bgfx/include/bgfx/c99/bgfx.h"
#include "bgfx/include/bgfx/platform.h";

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




void helloworld();
int init();
void quit();
int eventhandler();

extern SDL_Window *window;

extern SDL_Renderer *rend;




#endif