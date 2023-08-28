#include "engine.h"
#include "shaders.h"

#include "term.h"
#include "systems.h"

static GeoObject **rq;
static RenderQueue renderQueue1;
static RenderQueue renderQueue2;
static Program *this;


static float sensitivity = 0.1f;
static float speed = 2.0f;
static char firstMouse;
static float lastX = SCREEN_WIDTH / 2;
static float lastY = SCREEN_HEIGHT / 2;
static float yaw = -90.0f;
static float pitch = 0.0f;
static char captureMouse = 0;
static char initialized = false;

static unsigned int texNum = 0;

void testprogram_key_input_poll(void);


int testprogram_init()
{
    glfwSetWindowTitle(window, "test");

    terminal_clear();
    terminal_print("test program no.1 loaded\n");

    vec4 eye = {{0, 0, 5, 0}};
    vec4 center = {{0, 0, -1, 0}};

    c_pos = eye;
    c_front = center;

    if (initialized) return 0;

    skybox_load_texture("media/bitfs2.png");

    par_shapes_mesh *dodecahedron = par_shapes_create_dodecahedron();
    par_shapes_translate(dodecahedron, 0, 0.934, 0);

    par_shapes_mesh *tetrahedron = par_shapes_create_icosahedron();
    par_shapes_translate(tetrahedron, 1, 0, 3.5);

    par_shapes_mesh *octohedron = par_shapes_create_octahedron();
    par_shapes_translate(octohedron, -2.25, 0.9, -.5);

    par_shapes_mesh *icosahedron = par_shapes_create_icosahedron();
    par_shapes_translate(icosahedron, -1, 0.8, 3.5);

    par_shapes_mesh *cube = par_shapes_create_cube();
    par_shapes_rotate(cube, PAR_PI / 5.0, (float[]){0, 1, 0});
    par_shapes_translate(cube, 1, 0, 0.5);
    par_shapes_scale(cube, 1.2, 1.2, 1.2);

    par_shapes_mesh *mesh1 = par_shapes_create_empty();
    par_shapes_merge_and_free(mesh1, dodecahedron);
    par_shapes_merge_and_free(mesh1, icosahedron);
    par_shapes_merge_and_free(mesh1, cube);
    par_shapes__compute_welded_normals(mesh1);

    par_shapes_compute_normals(tetrahedron);
    par_shapes_compute_normals(octohedron);

    GeoObject *gobj = geo_obj_createFromParShape(mesh1);
    GeoObject *m1 = geo_obj_createFromParShape(tetrahedron);
    GeoObject *m2 = geo_obj_createFromParShape(octohedron);

    rq_init(&renderQueue1, 10);

    rq_init(&renderQueue2, 10);

    rq_add_object(&renderQueue2, geo_obj_createFromParShape(tetrahedron));

    rq_add_object(&renderQueue1, geo_obj_createFromParShape(octohedron));
    rq_add_object(&renderQueue1, geo_obj_createFromParShape(tetrahedron));

    Shader *s = newShaderObject(vertex_shader_0, fragment_shader_0);
    //gobj->shader = s;
    renderQueue1.gpuHandle.shader = s;

    s = newShaderObject(vertex_shader_2, fragment_shader_2);
    renderQueue2.gpuHandle.shader = s;

    rq = renderQueue1.objectBuffer;

    transform_set_identity(&gobj->baseTransform);
    transform_set_identity(&rq[0]->baseTransform);
    transform_set_identity(&rq[1]->baseTransform);

    renderQueue1.gpuHandle.textureAtlas = generateRandomAtlas();
    renderQueue2.gpuHandle.textureAtlas = generateRandomAtlas();

    gobj->baseTexture = 5;
    rq[0]->baseTexture = 2;
    rq[1]->baseTexture = 8;
    rq_add_object(&renderQueue1, gobj);

    geo_instanceop_init(rq[0], 10);
    Transform ta;
    Transform tb;
    Transform tc;
    transform_set_identity(&ta);
    transform_set_identity(&tb);
    transform_set_identity(&tc);
    transform_move(0, -1, 0, &ta);
    transform_move(0, 1, 0, &tb);
    transform_move(1, 0, 0, &tc);
    geo_instanceop_add(rq[0], ta.matrix, 4);
    geo_instanceop_add(rq[0], tb.matrix, 16);
    geo_instanceop_add(rq[0], tc.matrix, 16);


    par_shapes_mesh *shape = par_shapes_create_plane(3, 3);
    par_shapes_translate(shape, -0.5, 0, 1);
    par_shapes_scale(shape, 4, 1.5, 1);

    rq_add_object(&renderQueue1, geo_obj_createFromParShape(shape));

    GeoObject *cube1 = geo_new_object();
    geo_obj_loadFromFile("media/cube.obj", cube1);

    rq_add_object(&renderQueue1, cube1);

    initialized = true;
}

int testprogram_update(float deltaTime)
{
    testprogram_key_input_poll();
    //transform_rotate(0, 0, 1 * deltaTime, &rq[1]->baseTransform);
    //transform_rotate(1 * deltaTime, 0, 0, &rq[2]->baseTransform);

    // particle_update(p1);

    //rq[2]->baseTexture = rand() % 50;
    //rq[2]->instanceDirty = 1;

    rq_update_buffers(&renderQueue1);
    rq_update_buffers(&renderQueue2);

    render_skybox();

    geo_render(&renderQueue1.gpuHandle);
    geo_render(&renderQueue2.gpuHandle);
}

int testprogram_destroy()
{
    free(this);
}

int testprogram_keyCallback(int key, int action)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        program_pop();
    }
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        if (captureMouse)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            captureMouse = 0;
            return 0;
        }
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        captureMouse = 1;
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        texNum++;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        texNum--;
    }
}

int testprogram_mouseCallback(double xpos, double ypos)
{
    if (!captureMouse)
    {
        return 0;
    }
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

int testprogram_scrollCallback(double xoffset, double yoffset)
{
    fov = fov - (yoffset * 10);
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 120.0f) fov = 120.0f; 
    projectionMatrix = matrix_perspective(radians(fov), (float)s_width/s_height, 0.1f, 100.0f);
}

void testprogram_key_input_poll(void)
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
        fb_drawSineWave(10, 3.1f, appTime);
    }
    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
    {
        fb_copy_to_bg();
    }
}


Program *program_get_testmode()
{
    this = malloc(sizeof(Program));

    this->init = testprogram_init;
    this->update = testprogram_update;
    this->destroy = testprogram_destroy;

    this->keyCallback = testprogram_keyCallback;
    this->mouseCallback = testprogram_mouseCallback;
    this->scrollCallback = testprogram_scrollCallback;
    
    return this;
}