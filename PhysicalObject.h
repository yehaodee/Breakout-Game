#ifndef PHYSICALOBJECT_H
#define PHYSICALOBJECT_H

#include "raylib.h"
#include "GameObject.h"

class PhysicalObject : public virtual GameObject {
protected:
    Vector2 velocity;
    float radius;
public:
    PhysicalObject(Vector2 speed, float r);
    Vector2 GetVelocity() const { return velocity; }
    void SetVelocity(Vector2 vel) { velocity = vel; }
    float GetRadius() const { return radius; }
};

#endif

