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
#include <future>
#include <mutex>

enum GameState {
    MENU,
    PLAYING,
    GAME_OVER,
    VICTORY,
    PAUSED,
    LOADING
};
enum GameMode {
    SINGLE_PLAYER,
    TWO_PLAYER_HOST,
    TWO_PLAYER_CLIENT
};

// 网格空间划分常量
const int GRID_WIDTH = 8;   // 网格宽度
const int GRID_HEIGHT = 6;  // 网格高度

class Game {
protected:
    int score;
    int lives;
    int level;
    GameState currentState;
    GameMode gameMode;
    Paddle* localPaddle;
    Paddle* remotePaddle;
    std::future<void> loadingFuture;
    bool isLoading;
    std::mutex loadingMutex;

    // 网格空间划分
    float cellWidth;      // 每个网格单元的宽度
    float cellHeight;     // 每个网格单元的高度
    std::vector<Brick*> grid[GRID_WIDTH][GRID_HEIGHT];  // 网格数据结构

    void SimulateTextureLoading();
    void ResetGame();
    void UpdatePlayingState();
    void LoadTexture();
    void BuildGrid();     // 构建网格
    void ClearGrid();     // 清空网格
    void DrawBricksBatch();  // 批绘制砖块
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