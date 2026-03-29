#ifndef BRICK_H
#define BRICK_H

#include "raylib.h"
#include "Ball.h"

class Brick {
private:
    Rectangle rect;
    bool active;
    Color color;
    int health;
    int points;
public:
    Brick(float x, float y, float w, float h, Color col, int hp, int pts);
    void Draw();
    bool IsActive() { return active; }
    void SetActive(bool a) { active = a; }
    bool CheckCollision(Ball ball);
    int GetPoints() { return points; }
};

#endif