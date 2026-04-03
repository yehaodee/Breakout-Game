#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "raylib.h"

class GameObject {
protected:
    Vector2 position;
public:
    GameObject(Vector2 pos);
};

#endif