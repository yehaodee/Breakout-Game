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
enum GameMode {
    SINGLE_PLAYER,
    TWO_PLAYER_HOST,
    TWO_PLAYER_CLIENT
};

class Game {
protected:
    int score;
    int lives;
    int level;
    GameState currentState;
    GameMode gameMode;
    Paddle* localPaddle;
    Paddle* remotePaddle;
public:
    Paddle paddle;
    Paddle paddleTop;
    std::vector<Ball> balls;
    std::vector<Brick> bricks;
    std::vector<PowerUp> powerUps;
    std::vector<Particle> particles;
    float slowBallEffectTime;
    float ballSpeed;

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
    ~Game();
    void Init();
    virtual void Update(); 
    void Draw();
    void Shutdown();
    bool ShouldClose() const;
    void Close();
};

#endif