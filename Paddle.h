#ifndef PADDLE_H
#define PADDLE_H

#include "raylib.h"
#include "RectangleObject.h"
#include "VisualObject.h"

class Paddle : public RectangleObject, public VisualObject {
private:
    float originalWidth;
    float currentWidth;
    float effectRemainingTime;
public:
    Paddle(float x, float y, float w, float h);
    void Draw();
    void MoveLeft(float speed);
    void MoveRight(float speed);
    void MoveTo(float x, float y);
    void SetWidth(float w);
    Rectangle GetRect();
    void Extend(float extraWidth, float duration);
    void Update(float dt);
};

#endif