#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include <vector>

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
    
    void CreateBricks(int level);
    
public:
    Game();
    void Init();
    void Update();
    void Draw();
    void Shutdown();
};

#endif