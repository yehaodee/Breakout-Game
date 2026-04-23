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

void Paddle::MoveTo(float x, float y) {
    rect.x = x;
    rect.y = y;
}

Rectangle Paddle::GetRect() {
    return rect;
}

void Paddle::Extend(float extraWidth, float duration) {
    if (effectRemainingTime <= 0) {
        originalWidth = rect.width;
    }

    float centerX = rect.x + rect.width / 2;

    rect.width += extraWidth;
    currentWidth = rect.width;

    effectRemainingTime = duration;

    rect.x = centerX - rect.width / 2;
}

void Paddle::Update(float dt) {
    if (effectRemainingTime > 0) {
        effectRemainingTime -= dt;
        if (effectRemainingTime <= 0) {
            float centerX = rect.x + rect.width / 2;

            rect.width = originalWidth;
            currentWidth = originalWidth;

            rect.x = centerX - rect.width / 2;
        }
    }
}