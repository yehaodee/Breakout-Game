#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include <vector>
#include <string>

enum GameState {
    MENU,
    PLAYING,
    GAME_OVER,
    VICTORY,
    PAUSED
};

class Game {
private:
    Ball ball;
    Paddle paddle;
    std::vector<Brick> bricks;
    int score;
    int lives;
    int level;
    float ballSpeed;
    GameState currentState;
    float gameTime;
    
    // 配置参数
    int windowWidth;
    int windowHeight;
    std::string windowTitle;
    float ballRadius;
    float ballGravity;
    float ballMaxSpeed;
    float ballBounceForce;
    float paddleWidth;
    float paddleHeight;
    float paddleSpeed;
    float paddleBoostSpeed;
    int brickRows;
    int brickCols;
    float brickWidth;
    float brickHeight;
    int initialLives;
    int scorePerBrick;
    float timeMultiplierDecay;
    
    void CreateBricks(int level);
    void LoadConfig(const std::string& path);
    
public:
    Game();
    void Init();
    void Update();
    void Draw();
    void Shutdown();
};

#endif