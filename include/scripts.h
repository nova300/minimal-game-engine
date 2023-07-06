#ifndef scripts_h
#define scripts_h

int run_script(void* num);

int task_catch(TaskHandle *task);

int task_throw(TaskHandle *task);

int task_queue(TaskHandle *task);

typedef struct
{
    int cost;
    int programNr;
    char* callStruct;
    char* returnStruct;
}TaskHandle;


#endif