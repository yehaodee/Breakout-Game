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
    Rectangle GetRect();
    void Extend(float extraWidth, float duration);
    void Update(float dt);  // 每帧减少剩余时间，到期恢复
};

#endif