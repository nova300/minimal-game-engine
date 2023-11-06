#include <physics.h>
#include <geomath.h>
#include <stddef.h>

static Collider2D *colliders2d = NULL;
static int colliders2d_size;

void UpdateColliders(float deltaTime)
{
    if (colliders2d != NULL && colliders2d_size > 0)
    {
        for(int i = 0; i < colliders2d_size; i++)
        {
            
        }
    }
}


