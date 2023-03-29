#ifndef engine_h
#define engine_h


#ifdef _WIN32
#define ENTRYPOINT WinMain
#else
#define ENTRYPOINT main
#endif

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "graphics.h"

#define TITLE "minimal game engine"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

extern int errorCode;
extern SDL_Event e;
extern int exitLoop;

extern float degrees;

extern SDL_RendererFlip flipType;

extern Sprite *s1;



void helloworld();
int init();
void quit();
int eventhandler();

extern SDL_Window *window;

extern SDL_Renderer *rend;




#endif