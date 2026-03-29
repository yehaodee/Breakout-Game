#include "Brick.h"
#include "Ball.h"

Brick::Brick(float x, float y, float w, float h) {
    rect = { x, y, w, h };
    active = true;
}

void Brick::Draw() {
    if (active) {
        DrawRectangleRec(rect, GREEN);
    }
}

bool Brick::CheckCollision(Ball ball) {
    if (!active) return false;
    
    Vector2 ballPos = ball.GetPosition();
    float ballRadius = ball.GetRadius();
    
    if (CheckCollisionCircleRec(ballPos, ballRadius, rect)) {
        active = false;
        return true;
    }
    return false;
}
