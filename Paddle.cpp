#include "Paddle.h"

Paddle::Paddle(float x, float y, float w, float h) 
    : GameObject({x, y}), RectangleObject(x, y, w, h), VisualObject(BLUE) {
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
