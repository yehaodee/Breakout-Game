#include "Game.h"
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Game::Game()
    : score(0),
      lives(3),
      level(1),
      ballSpeed(3.0f),
      currentState(MENU),
      gameTime(0.0f),
      windowWidth(800),
      windowHeight(600),
      windowTitle("Breakout"),
      ballRadius(10),
      ballGravity(0.08f),
      ballMaxSpeed(15),
      ballBounceForce(0.5f),
      paddleWidth(120),
      paddleHeight(15),
      paddleSpeed(18),
      paddleBoostSpeed(28),
      brickRows(5),
      brickCols(8),
      brickWidth(85),
      brickHeight(25),
      initialLives(3),
      scorePerBrick(10),
      timeMultiplierDecay(0.05f),
      ball((Vector2){400, 300}, (Vector2){3, 3}, 10, RED),
      paddle(350, 550, 100, 20) {
    LoadConfig("config.json");
}

void Game::Init() {
    SetTargetFPS(60);
}

void Game::CreateBricks(int level) {
    bricks.clear();
    float startX = 50;
    float startY = 80;
    float gap = 10;
    
    Color colors[] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };
    int health[] = { 1, 1, 2, 2, 3, 3 };
    int points[] = { 10, 20, 30, 40, 50, 60 };
    
    for (int row = 0; row< brickRows; row++) {
        for (int col = 0; col < brickCols; col++) {
            float x = startX + col * (brickWidth + gap);
            float y = startY + row * (brickHeight + gap);
            int brickType = (row + level) % 6;
            bricks.emplace_back(x, y, brickWidth, brickHeight, colors[brickType], health[brickType], points[brickType]);
        }
    }
}

void Game::Update() {
    gameTime += GetFrameTime();
    
    if (currentState == MENU) {
        if (IsKeyPressed(KEY_ENTER)) {
            currentState = PLAYING;
            score = 0;
            lives = initialLives;
            level = 1;
            ballSpeed = 3;
            ball = Ball((Vector2){windowWidth / 2.0f, windowHeight / 2.0f}, (Vector2){ballSpeed, ballSpeed}, ballRadius, RED);
            CreateBricks(level);
        }
    }
    
    else if (currentState == PLAYING) {
        if (IsKeyPressed(KEY_P)) {
            currentState = PAUSED;
        }
        
        ball.Move();
        
        if (ball.BounceEdge(windowWidth, windowHeight)) {
            if (ball.GetPosition().y + ball.GetRadius() >= windowHeight) {
                lives--;
                if (lives<= 0) {
                    currentState = GAME_OVER;
                } else {
                    ball = Ball((Vector2){windowWidth / 2.0f, windowHeight / 2.0f}, (Vector2){ballSpeed, ballSpeed}, ballRadius, RED);
                }
            }
        }

        if (ball.CheckBrickCollision(bricks)) {
            score += ball.GetScoreValue();
        }
        
        if (IsKeyDown(KEY_LEFT)) paddle.MoveLeft(paddleSpeed);
        if (IsKeyDown(KEY_RIGHT)) paddle.MoveRight(paddleSpeed);

        bool allBricksDestroyed = true;
        for (auto& brick : bricks) {
            if (brick.IsActive()) {
                allBricksDestroyed = false;
                if (brick.CheckCollision(ball)) {
                    ball.ReverseY();
                    score += brick.GetPoints();
                }
            }
        }
        
        if (allBricksDestroyed) {
            level++;
            ballSpeed += 0.5;
            ball = Ball((Vector2){windowWidth / 2.0f, windowHeight / 2.0f}, (Vector2){ballSpeed, ballSpeed}, ballRadius, RED);
            CreateBricks(level);
            
            if (level >5) {
                currentState = VICTORY;
            }
        }

        ball.CheckPaddleCollision(paddle.GetRect());
    }
    
    else if (currentState == PAUSED) {
        if (IsKeyPressed(KEY_P)) {
            currentState = PLAYING;
        }
    }
    
    else if (currentState == GAME_OVER) {
        if (IsKeyPressed(KEY_ENTER)) {
            currentState = MENU;
        }
    }
    
    else if (currentState == VICTORY) {
        if (IsKeyPressed(KEY_ENTER)) {
            currentState = MENU;
        }
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (currentState == MENU) {
        DrawText("BREAKOUT GAME", 180, 200, 60, DARKBLUE);
        DrawText("Press ENTER to Start", 220, 300, 30, GRAY);
        DrawText("Press P to Pause", 280, 350, 20, GRAY);
    }
    
    else if (currentState == PLAYING || currentState == PAUSED) {
        ball.Draw();
        paddle.Draw();
        for (auto& brick : bricks) brick.Draw();
        
        DrawText(("Score: " + std::to_string(score)).c_str(), 20, 20, 20, DARKGRAY);
        DrawText(("Lives: " + std::to_string(lives)).c_str(), 150, 20, 20, DARKGRAY);
        DrawText(("Level: " + std::to_string(level)).c_str(), 300, 20, 20, DARKGRAY);
        
        if (currentState == PAUSED) {
            DrawRectangle(0, 0, windowWidth, windowHeight, Fade(BLACK, 0.5f));
            DrawText("PAUSED", windowWidth / 2 - 100, windowHeight / 2 - 20, 40, WHITE);
            DrawText("Press P to Resume", windowWidth / 2 - 100, windowHeight / 2 + 20, 20, WHITE);
        }
    }
    
    else if (currentState == GAME_OVER) {
        DrawText("GAME OVER!", windowWidth / 2 - 150, 200, 60, RED);
        DrawText(("Final Score: " + std::to_string(score)).c_str(), windowWidth / 2 - 100, 300, 30, GRAY);
        DrawText("Press ENTER to Restart", windowWidth / 2 - 120, 350, 30, GRAY);
    }
    
    else if (currentState == VICTORY) {
        DrawText("YOU WIN!", windowWidth / 2 - 120, 200, 60, GREEN);
        DrawText(("Final Score: " + std::to_string(score)).c_str(), windowWidth / 2 - 100, 300, 30, GRAY);
        DrawText("Press ENTER to Menu", windowWidth / 2 - 120, 350, 30, GRAY);
    }

    EndDrawing();
}

void Game::LoadConfig(const std::string& path) {
    std::ifstream f(path);
    json config = json::parse(f);
    
    windowWidth = config["window"]["width"];
    windowHeight = config["window"]["height"];
    windowTitle = config["window"]["title"];
    
    ballRadius = config["ball"]["radius"];
    ballGravity = config["ball"]["gravity"];
    ballMaxSpeed = config["ball"]["maxSpeed"];
    ballBounceForce = config["ball"]["bounceForce"];
    
    paddleWidth = config["paddle"]["width"];
    paddleHeight = config["paddle"]["height"];
    paddleSpeed = config["paddle"]["speed"];
    paddleBoostSpeed = config["paddle"]["boostSpeed"];
    
    brickRows = config["bricks"]["rows"];
    brickCols = config["bricks"]["cols"];
    brickWidth = config["bricks"]["width"];
    brickHeight = config["bricks"]["height"];
    
    initialLives = config["game"]["initialLives"];
    scorePerBrick = config["game"]["scorePerBrick"];
    timeMultiplierDecay = config["game"]["timeMultiplierDecay"];
}

void Game::Shutdown() {
    bricks.clear();
}