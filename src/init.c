#include "engine.h"

int init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return 1;
    }

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
    if (window == NULL)
    {
        return 2;
    }

    context = SDL_GL_CreateContext( window );
    if (context == NULL)
    {
        return 3;
    }

    glewExperimental = GL_TRUE;
    if ( glewInit() != GLEW_OK )
    {
        return 4;
    }

    if ( initGL() )
    {
        printf("gl init error\n");
        return 5;
    }
    
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        return 6;
    }

    if ( TTF_Init() == -1)
    {
        return 7;
    }
    

    return 0;
}


void quit()
{

    TTF_CloseFont( font );

    SDL_DestroyWindow( window );
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int initGL()
{
    GLenum error = GL_NO_ERROR;

    if (SDL_GL_SetSwapInterval( 1 ) < 0 )
    {
        printf("non fatal error: could not set vsync\n");
    }

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    return 0;
}


