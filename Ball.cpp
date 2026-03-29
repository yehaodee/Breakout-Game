#include "Ball.h"

Ball::Ball(Vector2 pos, Vector2 sp, float r) {
    position = pos;
    speed = sp;
    radius = r;
}

void Ball::Move() {
    position.x += speed.x;
    position.y += speed.y;
}

void Ball::Draw() {
    DrawCircleV(position, radius, RED);
}

bool Ball::BounceEdge(int screenWidth, int screenHeight) {
    bool hitBottom = false;
    
    // 左右边界
    if (position.x - radius<= 0 || position.x + radius >= screenWidth) {
        speed.x *= -1;
    }
    // 上边界
    if (position.y - radius<= 0) {
        speed.y *= -1;
    }
    // 下边界（球掉落）
    if (position.y + radius >= screenHeight) {
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
    speed.y *= -1;
}

bool Ball::CheckPaddleCollision(Rectangle paddleRect) {
    if (CheckCollisionCircleRec(position, radius, paddleRect)) {
        // 反转Y方向速度
        speed.y *= -1;
        
        // 计算碰撞点相对于 paddle 中心的偏移量
        float hitPos = position.x - (paddleRect.x + paddleRect.width / 2);
        float normalizedHitPos = hitPos / (paddleRect.width / 2);
        
        // 根据碰撞位置调整X方向速度，实现不同角度的反弹
        speed.x = normalizedHitPos * 5.0f;
        
        // 确保球不会卡在 paddle 内
        position.y = paddleRect.y - radius - 1;
        
        return true;
    }
    return false;
}
