#include "engine.h"

unsigned long time = 0;
unsigned long deltaTime = 0;

SDL_Window *window;
SDL_GLContext *context;

TTF_Font *font;

ParticleSystem *p1;

int exitLoop = 0;
float degrees = 0;


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
    

    while (exitLoop == 0)
    {

        deltaTime = SDL_GetTicks() - time;
        eventhandler();
        if (deltaTime > 250) deltaTime = 250;
        time = SDL_GetTicks();
        
        glClear( GL_COLOR_BUFFER_BIT );

        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        glTranslatef( SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f, 0.f );

        glBegin( GL_QUADS );
            glColor3f( 1.f, 0.f, 0.f ); glVertex2f( -50.f, -50.f );
            glColor3f( 1.f, 1.f, 0.f ); glVertex2f(  50.f, -50.f );
            glColor3f( 0.f, 1.f, 0.f ); glVertex2f(  50.f,  50.f );
            glColor3f( 0.f, 0.f, 1.f ); glVertex2f( -50.f,  50.f );
        glEnd();

        SDL_GL_SwapWindow( window );
    }


    quit();
    printf("Goodbye.\n");
    return 0;
}