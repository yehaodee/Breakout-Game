#ifndef PADDLE_H
#define PADDLE_H

#include "raylib.h"
#include "RectangleObject.h"
#include "VisualObject.h"

class Paddle : public RectangleObject, public VisualObject {
public:
    Paddle(float x, float y, float w, float h);
    void Draw();
    void MoveLeft(float speed);
    void MoveRight(float speed);
    Rectangle GetRect();
};

#endif