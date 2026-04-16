#include "Paddle.h"

Paddle::Paddle(float x, float y, float w, float h) 
    : GameObject({x, y}), RectangleObject(x, y, w, h), VisualObject(BLUE) {
    originalWidth = w;
    currentWidth = w;
    effectRemainingTime = 0.0f;
}

void Paddle::Draw() {
    DrawRectangleRec(rect, color);
}

void Paddle::MoveLeft(float speed) {
    rect.x -= speed;
    if (rect.x < 0) rect.x = 0;
}

void Paddle::MoveRight(float speed) {
    rect.x += speed;
    if (rect.x + rect.width > GetScreenWidth())
        rect.x = GetScreenWidth() - rect.width;
}

Rectangle Paddle::GetRect() { 
    return rect;
}

void Paddle::Extend(float extraWidth, float duration) {
    // 保存原始宽度（如果还没有保存）
    if (effectRemainingTime <= 0) {
        originalWidth = rect.width;
    }
    
    // 保存当前的中心位置
    float centerX = rect.x + rect.width / 2;
    
    // 增加 paddle 宽度
    rect.width += extraWidth;
    currentWidth = rect.width;
    
    // 重置效果持续时间
    effectRemainingTime = duration;
    
    // 调整 paddle 位置，保持中心位置不变
    rect.x = centerX - rect.width / 2;
}

void Paddle::Update(float dt) {
    if (effectRemainingTime > 0) {
        effectRemainingTime -= dt;
        if (effectRemainingTime <= 0) {
            // 保存当前的中心位置
            float centerX = rect.x + rect.width / 2;
            
            // 恢复原始宽度
            rect.width = originalWidth;
            currentWidth = originalWidth;
            
            // 调整 paddle 位置，保持中心位置不变
            rect.x = centerX - rect.width / 2;
        }
    }
}
