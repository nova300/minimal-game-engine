#ifndef physics_h
#define physics_h

#include <geomath.h>
#include <stdbool.h>

typedef unsigned char objectid;

typedef struct
{
    bool active;
    objectid id;
    vec2 position;
    vec2 velocity;
    float radius;
    bool collisionFlag;
    vec2 collisionVector;
}Collider2D;

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