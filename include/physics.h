#ifndef physics_h
#define physics_h

#include <geomath.h>
#include <stdbool.h>

typedef int objectid;

typedef struct
{
    vec2 vector;
    unsigned char channel;
}collisionData;


typedef struct
{
    bool active;
    objectid id;
    unsigned char channel;
    vec2 position;
    int width;
    int height;
    float radius;
    bool collisionFlag;
    collisionData collision;
}Collider2D;

objectid GetNewCollider();
void UpdateColliders(float deltaTime);

typedef struct
{
    bool active;
    objectid id;
    vec2 velocity;
    bool collision;
    objectid collider_id;
}VelocityMovement2D;





#endif