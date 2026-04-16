#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"
#include "PowerUpEffect.h"
#include "PowerUp.h"
#include "Particle.h"
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
    int score;
    int lives;
    int level;
    GameState currentState;
    float gameTime;
public:
    Paddle paddle;
    std::vector<Ball> balls;
    std::vector<Brick> bricks;
    std::vector<PowerUp> powerUps;
    std::vector<Particle> particles;
    float slowBallEffectTime;
    float ballSpeed;
    
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
    
    // 道具配置
    struct PowerUpConfig {
        float extraWidth;
        float duration;
        float dropRate;
        int extraBalls;
        float speedFactor;
    };
    PowerUpConfig powerUpConfig[3];
    
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