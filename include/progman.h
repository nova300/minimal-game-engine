#ifndef progman_h
#define progman_h

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




/* PROGRAMS */
typedef int (*functionPointer)();
typedef int (*functionPointerFloat)(float);
typedef int (*functionPointerDoubleDouble)(double, double);
typedef int (*functionPointerIntInt)(int, int);

typedef struct
{
    functionPointer init;
    functionPointerFloat update;
    functionPointer destroy;
    functionPointerIntInt keyCallback;
    functionPointerDoubleDouble mouseCallback;
    functionPointerDoubleDouble scrollCallback;
}Program;

extern Program **programStack;
extern int programCapacity;
extern int programTop;

int program_init(void);
int program_free(void);
int program_push(Program *program);
int program_pop(void);
int program_update(float deltaTime);
Program *program_get(void);

/* BEHAVIOURS */


#endif