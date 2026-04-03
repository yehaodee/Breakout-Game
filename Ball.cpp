#include "Ball.h"
#include "Brick.h"

Ball::Ball(Vector2 pos, Vector2 sp, float r, Color c) : GameObject(pos), PhysicalObject(sp, r), VisualObject(c) {
    scoreValue = 0;
}

void Ball::Move() {
    position.x += velocity.x;
    position.y += velocity.y;
}

void Ball::Draw() {
    DrawCircleV(position, GetRadius(), color);
}

bool Ball::BounceEdge(int screenWidth, int screenHeight) {
    bool hitBottom = false;
    
    // 左右边界
    if (GetPosition().x - GetRadius()<= 0 || GetPosition().x + GetRadius() >= screenWidth) {
        velocity.x *= -1;
    }
    // 上边界
    if (GetPosition().y - GetRadius()<= 0) {
        velocity.y *= -1;
    }
    // 下边界（球掉落）
    if (GetPosition().y + GetRadius() >= screenHeight) {
        hitBottom = true;
    }
    
    return hitBottom;
}

Vector2 Ball::GetPosition() { 
    return position;
}

float Ball::GetRadius() { 
    return radius;
}

void Ball::ReverseY() { 
    velocity.y *= -1;
}

bool Ball::CheckPaddleCollision(Rectangle paddleRect) {
    if (CheckCollisionCircleRec(GetPosition(), GetRadius(), paddleRect)) {
        // 反转Y方向速度
        velocity.y *= -1;
        
        // 计算碰撞点相对于 paddle 中心的偏移量
        float hitPos = GetPosition().x - (paddleRect.x + paddleRect.width / 2);
        float normalizedHitPos = hitPos / (paddleRect.width / 2);
        
        // 根据碰撞位置调整X方向速度，实现不同角度的反弹
        velocity.x = normalizedHitPos * 5.0f;
        
        // 确保球不会卡在 paddle 内
        position.y = paddleRect.y - GetRadius() - 1;   
        
        return true;
    }
    return false;
}

bool Ball::CheckBrickCollision(std::vector<class Brick>& bricks) {
    for (auto& brick : bricks) {
        if (brick.IsActive() && brick.CheckCollision(*this)) {
            scoreValue = brick.GetPoints();
            return true;
        }
    }
    scoreValue = 0;
    return false;
}

int Ball::GetScoreValue() {
    return scoreValue;
}
