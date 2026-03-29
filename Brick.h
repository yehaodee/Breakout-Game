#ifndef BRICK_H
#define BRICK_H

#include "raylib.h"
#include "Ball.h"

class Brick {
private:
    Rectangle rect;
    bool active;
public:
    Brick(float x, float y, float w, float h);
    void Draw();
    bool IsActive() { return active; }
    void SetActive(bool a) { active = a; }
    bool CheckCollision(Ball ball);
};

#endif