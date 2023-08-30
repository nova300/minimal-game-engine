#include "engine.h"
#include "shaders.h"

#include "term.h"
#include "systems.h"

//#include "thread.h"
#include <pthread.h>
#include <stdatomic.h>

static GeoObject **rq;
static RenderQueue renderQueue1;
static Program *this;


static float sensitivity = 0.1f;
static float mspeed = 2.0f;
static char firstMouse;
static float lastX = SCREEN_WIDTH / 2;
static float lastY = SCREEN_HEIGHT / 2;
static float yaw = -90.0f;
static float pitch = 0.0f;
static char captureMouse = 0;

static ParticleSystem *p1;

void boidprogram_key_input_poll(void);

typedef struct Boid
{
    struct Boid **localBoidList;
    int localBoidListAmount;
    Transform transform;
    vec4 direction;
    float radius;
    float steerSpeed;
    float speed;
    int texture;
    int id;
}boid;

static boid *boids;
static Transform *transform;
static int amount;

static float speed;
static float steerSpeed;
static float radius;

static float separationWeight = 0.30;
static float alignmentWeight = 0.145;

char scrollmode = 0;

static vec4 zero;

static GeoObject *gobj;

static float fpstimer = 0;
static float fps = 999;
static int fpscounter = 0;

static char initialized = false;

//static thread_atomic_int_t threadStatus;

//static thread_ptr_t thread;

static pthread_t thread;

static atomic_int threadStatus;


int boidprogram_init()
{   

    glfwSetWindowTitle(window, "boids");

    terminal_clear();
    terminal_print("boid program loaded\n");

    fb_load_bg("media/underwater2.png", true);

    vec4 eye = {{0, 0, 50, 0}};
    vec4 center = {{0, 0, -1, 0}};

    c_pos = eye;
    c_front = center;

    if (initialized) return 0;

    par_shapes_mesh *mesh1 = par_shapes_create_tetrahedron();

    par_shapes_compute_normals(mesh1);

    gobj = geo_obj_createFromParShape(mesh1);

    rq_init(&renderQueue1, 10);


    Shader *s = newShaderObject(vertex_shader_0, fragment_shader_0);
    renderQueue1.gpuHandle.shader = s;

    rq = renderQueue1.objectBuffer;

    transform_set_identity(&gobj->baseTransform);

    renderQueue1.gpuHandle.textureAtlas = generateRandomAtlas();

    gobj->baseTexture = 5;

    rq_add_object(&renderQueue1, gobj);

    //p1 = particle_new(gobj, 100);

    amount = 2000;

    int len = snprintf(NULL, 0, "%d", amount);
    char *result = malloc(len + 1);
    snprintf(result, len + 1, "%d", amount);
    terminal_print(result);
    terminal_print(" boids\n");
    free(result);

    radius = 10.0f;
    steerSpeed = 0.01f;
    speed = 10.0f;

    geo_instanceop_init(gobj, amount);

    transform = &(gobj->baseTransform);
    boid *b = malloc(sizeof(boid) * amount);

    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    zero.w = 0;

    for (int i = 0; i < amount; i++)
    {
        b[i].radius = radius;
        b[i].speed = speed;
        b[i].steerSpeed = steerSpeed;
        b[i].direction.x = ((rand() - rand()) % 3) + ((rand() - rand()) % 10);
        b[i].direction.y = ((rand() - rand()) % 3) + ((rand() - rand()) % 10);
        b[i].direction.z = ((rand() - rand()) % 3) + ((rand() - rand()) % 10);
        b[i].texture = rand() % 50;
        transform_set_identity(&b[i].transform);

        transform_position(((rand() - rand()) % 3) + ((rand() - rand()) % 10), ((rand() - rand()) % 3) + ((rand() - rand()) % 10), ((rand() - rand()) % 3) + ((rand() - rand()) % 10), &b[i].transform);
        
        b[i].localBoidList = malloc(sizeof(boid*) * 40);
        b[i].localBoidListAmount = 0;

        b[i].id = i;

    }
    
    boids = b;

    //thread_atomic_int_store( &threadStatus, 0 );

    initialized = true;
}

void updateLocalBoidList(boid *b)
{
    b->localBoidListAmount = 0;

    for (int i = 0; i < amount; i++)
    {
        if (b->localBoidListAmount == 40) break;
        if (boids[i].id == b->id) continue;

        if (vector_distance(b->transform.position, boids[i].transform.position) < radius)
        {
            b->localBoidList[b->localBoidListAmount] = &boids[i];
            b->localBoidListAmount++;
        }
    }
}

void doCohesion(boid *b)
{
    vec4 avg = zero;
    if (b->localBoidListAmount == 0) return;

    for (int i = 0; i < b->localBoidListAmount; i++)
    {
        avg = vector_add(avg, b->localBoidList[i]->transform.position);
    }

    avg.x = avg.x / b->localBoidListAmount;
    avg.y = avg.y / b->localBoidListAmount;
    avg.z = avg.z / b->localBoidListAmount;

    vec4 dir = vector_subtract(avg, b->transform.position);

    vector_normalize(&dir);

    b->direction.x += dir.x * steerSpeed;
    b->direction.y += dir.y * steerSpeed;
    b->direction.z += dir.z * steerSpeed;
}

void doAlignment(boid *b)
{
    vec4 avg = zero;
    if (b->localBoidListAmount == 0) return;

    for (int i = 0; i < b->localBoidListAmount; i++)
    {
        avg = vector_add(avg, b->localBoidList[i]->direction);
    }

    avg.x = avg.x / b->localBoidListAmount;
    avg.y = avg.y / b->localBoidListAmount;
    avg.z = avg.z / b->localBoidListAmount;

    float alignSpeed = steerSpeed * alignmentWeight;

    b->direction.x += avg.x * alignSpeed;
    b->direction.y += avg.y * alignSpeed;
    b->direction.z += avg.z * alignSpeed;
}

void doSeperation(boid *b)
{
    float sepRad = radius * 0.8f;
    //boid *closeBoids[50];
    int closeBoidsAmount = 0;
    vec4 avg = zero;
    
    for (int i = 0; i < amount; i++)
    {
        if (closeBoidsAmount == 49) break;
        float dist = vector_distance(b->transform.position, boids[i].transform.position);
        if (dist < sepRad)
        {
            //closeBoids[closeBoidsAmount] = &boids[i];
            closeBoidsAmount++;
            //sepRad = dist;

            vec4 diff;

            diff.x = boids[i].transform.position.x;
            diff.y = boids[i].transform.position.y;
            diff.z = boids[i].transform.position.z;

            /*if (dist > 1.0f)
            {
                dist = 1.0f;
            }

            diff.x = diff.x - (diff.x * dist);
            diff.y = diff.y - (diff.y * dist);
            diff.z = diff.z - (diff.z * dist); */

            avg = vector_add(avg, diff);
        }
    }

    if (closeBoidsAmount == 0)
    {
        return;
    }

    avg.x = avg.x / closeBoidsAmount;
    avg.y = avg.y / closeBoidsAmount;
    avg.z = avg.z / closeBoidsAmount;

    vec4 dir = vector_subtract(avg, b->transform.position);

    float separationSpeed = steerSpeed * separationWeight;

    b->direction.x -= dir.x * separationSpeed;
    b->direction.y -= dir.y * separationSpeed;
    b->direction.z -= dir.z * separationSpeed;
}

void doRetention(boid *b)
{
    float retentionDist = 40.0f;
    if (vector_distance(transform->position, b->transform.position) > (retentionDist * 2))
    {
        transform_position(0.0f, 0.0f, 0.0f, &b->transform);
    }

    if (vector_distance(transform->position, b->transform.position) > retentionDist)
    {
        vec4 dir = vector_subtract(b->transform.position, transform->position);

        b->direction.x -= dir.x * steerSpeed * 2;
        b->direction.y -= dir.y * steerSpeed * 2;
        b->direction.z -= dir.z * steerSpeed * 2;
    }
}

void *update_boids(void* arg)
{
    //thread_set_high_priority();
    //float deltaTime = 0;
    //deltaTime = *(float*)in_deltaTime;
    int i;
    #pragma omp parallel for
    for (i = 0; i < amount; i++)
    {
        updateLocalBoidList(&boids[i]);
        doAlignment(&boids[i]);
        doCohesion(&boids[i]);
        doSeperation(&boids[i]);
        doRetention(&boids[i]);
        vector_normalize(&boids[i].direction);
        transform_move(boids[i].direction.x * (deltaTime * speed), boids[i].direction.y * (deltaTime * speed), boids[i].direction.z * (deltaTime * speed), &boids[i].transform);
        transform_set_rotation(boids[i].direction.x, boids[i].direction.y, boids[i].direction.z, &boids[i].transform);
        transform_make_matrix(&boids[i].transform);
        //geo_instanceop_add(gobj, boids[i].transform.matrix, boids[i].texture);
    }
    threadStatus = 0;
    //thread_exit(EXIT_SUCCESS);
}

int boidprogram_update(float deltaTime)
{
    boidprogram_key_input_poll();

    if (threadStatus == 0)
    {
        threadStatus = 1;
        geo_instanceop_clear(gobj);
        for (int i = 0; i < amount; i++)
        {
            geo_instanceop_add(gobj, boids[i].transform.matrix, boids[i].texture);
        }
        rq_update_buffers(&renderQueue1);

        pthread_create(&thread, NULL, update_boids, NULL);
        pthread_detach(thread);
    }

    geo_render(&renderQueue1.gpuHandle);



    if (fpstimer > 1)
    {
        fps = fpstimer / fpscounter;
        fps = 1 / fps;
        fpscounter = 0;
        fpstimer = 0;
    }
    else
    {
        fpstimer = fpstimer + deltaTime;
        fpscounter++;
    }
    int len = snprintf(NULL, 0, "%3.0f", fps);
    char *result = malloc(len + 1);
    snprintf(result, len + 1, "%3.0f", fps);
    terminal_display(result);
    free(result);
}

int boidprogram_destroy()
{
    free(this);
}

int boidprogram_keyCallback(int key, int action)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        program_pop();
    }
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
       for (int i = 0; i < amount; i++)
        {
            transform_position(((rand() - rand()) % 30) + ((rand() - rand()) % 100), ((rand() - rand()) % 30) + ((rand() - rand()) % 100), ((rand() - rand()) % 30) + ((rand() - rand()) % 100), &boids[i].transform);
        } 
        terminal_clear();
        terminal_print("reset simulation\n");
    }
}

int boidprogram_mouseCallback(double xpos, double ypos)
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

int boidprogram_scrollCallback(double xoffset, double yoffset)
{
    if (scrollmode == 0)
    {
        fov = fov - (yoffset * 10);
        if (fov < 1.0f)
        fov = 1.0f;
        if (fov > 120.0f)
        fov = 120.0f;
        projectionMatrix = matrix_perspective(radians(fov), (float)s_width / s_height, 0.1f, 100.0f);
        return 0;
    }
    if (scrollmode == 1)
    {
        alignmentWeight += (yoffset * 0.01f);
        printf("alignment weight: %f\n", alignmentWeight);
    }

    if (scrollmode == 2)
    {
        separationWeight += (yoffset * 0.01f);
        printf("separation weight: %f\n", separationWeight);
    }
}

void boidprogram_key_input_poll(void)
{
    float c_speed = mspeed * deltaTime;
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
        scrollmode = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
    {
        scrollmode = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
    {
        scrollmode = 2;
    }
    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
    {
        scrollmode = 3;
    }
}


Program *program_get_boidmode()
{
    this = malloc(sizeof(Program));

    this->init = boidprogram_init;
    this->update = boidprogram_update;
    this->destroy = boidprogram_destroy;

    this->keyCallback = boidprogram_keyCallback;
    this->mouseCallback = boidprogram_mouseCallback;
    this->scrollCallback = boidprogram_scrollCallback;

    return this;
}