#include <physics.h>
#include <geomath.h>
#include <stddef.h>
#include <stdlib.h>

static Collider2D *colliders2d = NULL;
static int colliders2d_size = -1;

void ResetCollider(Collider2D *col)
{
    vec2 zero;
    zero.x = 0.0f;
    zero.y = 0.0f;
    col->active = true;
    col->collisionFlag = false;
    col->collisionVector = zero;
    col->position = zero;
    col->radius = 0.1f;
    col->id = 0;
}

bool checkCollision(Collider2D *colA, Collider2D *colB)
{
    float leftA, leftB;
    float rightA, rightB;
    float topA, topB;
    float bottomA, bottomB;

    leftA = colA->position.x;
    rightA = colA->position.x + colA->width;
    topA = colA->position.y;
    bottomA = colA->position.y + colA->height;

    leftB = colB->position.x;
    rightB = colB->position.x + colB->width;
    topA = colB->position.y;
    bottomB = colB->position.y + colB->height;

    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

objectid GetNewCollider()
{
    if (colliders2d == NULL)
    {
        colliders2d = malloc(sizeof(Collider2D) * 64);
        colliders2d_size = 64;
        ResetCollider(colliders2d + 0);
        return 0;
    }
    else if (colliders2d_size > 0)
    {
        for(int i = 0; i < colliders2d_size; i++)
        {
            if (colliders2d[i].active) continue;
            ResetCollider(colliders2d + i);
            return i;
        }
        //grow collider array here
    }

    return -1; //failure
}

void UpdateColliders(float deltaTime)
{
    if (colliders2d != NULL && colliders2d_size > 0)
    {
        for(int i = 0; i < colliders2d_size; i++)
        {
            if(!colliders2d[i].active) continue;
            Collider2D* near = colliders2d + i;

            for(int j = 0; j < colliders2d_size; i++)
            {
                if(!colliders2d[j].active) continue;
                if(colliders2d[j].collisionFlag) continue;
                Collider2D* far = colliders2d + j;
                if(checkCollision(far, near))
                {
                    far->collisionFlag = true;
                    near->collisionFlag = true;
                    far->collisionChannel = near->channel;
                    near->collisionChannel = far->channel;
                    vec2 dir = vector2_subtract(far->position, near->position);
                    vector2_normalize(&dir);
                    near->collisionVector = dir;
                    dir = vector2_subtract(near->position, far->position);
                    vector2_normalize(&dir);
                    far->collisionVector = dir;
                }
            }
        }
    }
}


