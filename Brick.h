#ifndef BRICK_H
#define BRICK_H

#include "raylib.h"
#include "RectangleObject.h"
#include "VisualObject.h"
#include "Ball.h"

class Brick : public RectangleObject, public VisualObject {
private:
    int health;
    int points;
public:
    Brick(float x, float y, float w, float h, Color col, int hp, int pts);
    void Draw();
    bool IsActive() { return visible; }
    void SetActive(bool a) { visible = a; }
    bool CheckCollision(Ball ball);
    int GetPoints() { return points; }
};

#endif