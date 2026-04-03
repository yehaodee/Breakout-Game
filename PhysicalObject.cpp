#include "PhysicalObject.h"

PhysicalObject::PhysicalObject(Vector2 speed, float r) : GameObject({0, 0}) {
    velocity = speed;
    radius = r;
}
