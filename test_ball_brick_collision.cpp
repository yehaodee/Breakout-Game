#include "Ball.h"
#include "Brick.h"
#include <vector>
#include <cassert>

void TestBallBrickCollision_Hit() {
    // 创建球和砖块，确保它们碰撞
    Ball ball((Vector2){100, 100}, (Vector2){0, 0}, 10, RED);
    std::vector<Brick> bricks;
    bricks.emplace_back(90, 90, 30, 30, BLUE, 1, 10);
    
    bool result = ball.CheckBrickCollision(bricks);
    
    assert(result == true && "球应该与砖块碰撞");
    assert(ball.GetScoreValue() == 10 && "碰撞后分数值应该为砖块的分数");
}

void TestBallBrickCollision_NoHit() {
    // 创建球和砖块，确保它们不碰撞
    Ball ball((Vector2){100, 100}, (Vector2){0, 0}, 10, RED);
    std::vector<Brick> bricks;
    bricks.emplace_back(200, 200, 30, 30, BLUE, 1, 10);
    
    bool result = ball.CheckBrickCollision(bricks);
    
    assert(result == false && "球不应该与砖块碰撞");
    assert(ball.GetScoreValue() == 0 && "未碰撞时分数值应该为0");
}

void TestBallBrickCollision_MultipleBricks() {
    // 创建球和多个砖块，测试只与一个砖块碰撞
    Ball ball((Vector2){100, 100}, (Vector2){0, 0}, 10, RED);
    std::vector<Brick> bricks;
    bricks.emplace_back(90, 90, 30, 30, BLUE, 1, 10);
    bricks.emplace_back(200, 200, 30, 30, GREEN, 1, 20);
    
    bool result = ball.CheckBrickCollision(bricks);
    
    assert(result == true && "球应该与第一个砖块碰撞");
    assert(ball.GetScoreValue() == 10 && "应该返回第一个砖块的分数");
}

void TestBallBrickCollision_InactiveBrick() {
    // 创建球和不活跃的砖块，测试不碰撞
    Ball ball((Vector2){100, 100}, (Vector2){0, 0}, 10, RED);
    std::vector<Brick> bricks;
    bricks.emplace_back(90, 90, 30, 30, BLUE, 1, 10);
    bricks[0].SetActive(false);
    
    bool result = ball.CheckBrickCollision(bricks);
    
    assert(result == false && "球不应该与不活跃的砖块碰撞");
    assert(ball.GetScoreValue() == 0 && "未碰撞时分数值应该为0");
}

int main() {
    TestBallBrickCollision_Hit();
    TestBallBrickCollision_NoHit();
    TestBallBrickCollision_MultipleBricks();
    TestBallBrickCollision_InactiveBrick();
    
    return 0;
}