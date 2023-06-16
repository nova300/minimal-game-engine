#include "engine.h"

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

int init()
{
    //SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    //SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3);
    //SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        return 4;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE, NULL, NULL);

    //window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
    {
        return 2;
    }

    glfwMakeContextCurrent(window);

    /*context = SDL_GL_CreateContext( window );
    if (context == NULL)
    {
        return 3;
    }*/

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
    
    return 0;
}


void quit()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

int initGL()
{
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    glfwSwapInterval(1);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    return 0;
}


