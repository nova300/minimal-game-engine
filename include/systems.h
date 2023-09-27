#ifndef systems_h
#define systems_h


typedef int (*functionPointer)();
typedef int (*functionPointerFloat)(float);
typedef int (*functionPointerInt)(int);
typedef int (*functionPointerDoubleDouble)(double, double);
typedef int (*functionPointerIntInt)(int, int);

/* TASKS */

int run_script(void* num);

typedef struct
{
    int cost;
    int programNr;
    char* callStruct;
    char* returnStruct;
}TaskHandle;

int task_catch(TaskHandle *task);

int task_throw(TaskHandle *task);

int task_queue(TaskHandle *task);


/* SCENES */

typedef struct
{
    functionPointer init;
    functionPointerFloat update;
    functionPointer destroy;
    functionPointerIntInt keyCallback;
    functionPointerDoubleDouble mouseCallback;
    functionPointerDoubleDouble scrollCallback;
}Scene;

extern Scene **sceneStack;
extern int sceneCapacity;
extern int sceneTop;

int scene_init(void);
int scene_free(void);
int scene_push(Scene *program);
int scene_pop(void);
int scene_update(float deltaTime);
Scene *scene_get(void);

Scene *scene_get_selftest();

/* BEHAVIOURS */

#define MAXBEHAVIOURS 100

typedef struct
{
    /* -- base -- */
    functionPointer add;
    functionPointer remove;
    char active;
    unsigned char type;
    /* -- instance -- */
    functionPointerInt init;
    functionPointerFloat update;
    functionPointerInt destroy;
    void *dataStore;
    unsigned int *entities;
    int instanceCount;
    int instanceTop;
}Behaviour;

typedef int (*functionPointerBehaviourPointer)(Behaviour*);

int behaviour_init(void);
int behaviour_update(float deltaTime);
Behaviour *behaviour_get(int id);

#endif