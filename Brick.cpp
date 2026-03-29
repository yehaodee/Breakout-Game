#include "Brick.h"
#include "Ball.h"

Brick::Brick(float x, float y, float w, float h, Color col, int hp, int pts) {
    rect = { x, y, w, h };
    active = true;
    color = col;
    health = hp;
    points = pts;
}

void Brick::Draw() {
    if (active) {
        DrawRectangleRec(rect, color);
        // 根据生命值显示不同的纹理效果
        if (health > 1) {
            DrawRectangleLinesEx(rect, 2, DARKGRAY);
        }
    }
}

bool Brick::CheckCollision(Ball ball) {
    if (!active) return false;
    
    Vector2 ballPos = ball.GetPosition();
    float ballRadius = ball.GetRadius();
    
    if (CheckCollisionCircleRec(ballPos, ballRadius, rect)) {
        health--;
        if (health<= 0) {
            active = false;
        }
        return true;
    }
    return false;
}
