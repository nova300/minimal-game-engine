#include "engine.h"

float sensitivity = 0.1f;
float speed = 2.0f;

char firstMouse;
float lastX = SCREEN_WIDTH / 2;
float lastY = SCREEN_HEIGHT / 2;
float yaw = -90.0f;
float pitch = 0.0f;

char captureMouse = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        if (captureMouse)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            captureMouse = 0;
            return;
        }
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        captureMouse = 1;
    }


}

void key_input_poll(void)
{
    float c_speed = speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        c_speed = c_speed * 2;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        c_pos = vector_add(c_pos, vector_scale(c_front, c_speed));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        c_pos = vector_subtract(c_pos, vector_scale(c_front, c_speed));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {   
        vec4 m = vector_cross(c_front, c_up);
        vector_normalize(&m);
        c_pos = vector_subtract(c_pos, vector_scale(m, c_speed));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        vec4 m = vector_cross(c_front, c_up);
        vector_normalize(&m);
        c_pos = vector_add(c_pos, vector_scale(m, c_speed));
    }

    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
    {
        fb_test_pattern();
    }
    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
    {
        fb_test_dot();
    }
    if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
    {
        fb_drawSineWave(10, 3.1f, time);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    vec4 direction;
    direction.x = cos(radians(yaw)) * cos(radians(pitch));
    direction.y = sin(radians(pitch));
    direction.z = sin(radians(yaw)) * cos(radians(pitch));
    direction.w = 0;
    vector_normalize(&direction);
    c_front = direction;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov = fov - (yoffset * 10);
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 120.0f) fov = 120.0f; 
}


void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

