#ifndef RECTANGLE_OBJECT_H
#define RECTANGLE_OBJECT_H

#include "GameObject.h"
#include "raylib.h"

class RectangleObject : public virtual GameObject {
protected:
    Rectangle rect;
public:
    RectangleObject(float x, float y, float w, float h);
    Rectangle GetRect() const { return rect; }
};
#endif