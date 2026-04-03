#ifndef BALL_H
#define BALL_H

#include "raylib.h"
#include "PhysicalObject.h"
#include "VisualObject.h"
#include <vector>

class Ball : public PhysicalObject, public VisualObject {
private:
    int scoreValue;
public:
    Ball(Vector2 pos, Vector2 sp, float r, Color c);
    void Move();
    void Draw();
    bool BounceEdge(int screenWidth, int screenHeight);
    Vector2 GetPosition();
    float GetRadius();
    void ReverseY();
    bool CheckPaddleCollision(Rectangle paddleRect);
    bool CheckBrickCollision(std::vector<class Brick>& bricks);
    int GetScoreValue();
};

#endif