#include "Game.h"
#include <string>

Game::Game()
    : ball((Vector2){400, 300}, (Vector2){3, 3}, 10, RED),
      paddle(350, 550, 100, 20),
      score(0),
      lives(3),
      level(1),
      ballSpeed(3.0f),
      currentState(MENU),
      gameTime(0.0f) {}

void Game::Init() {
    SetTargetFPS(60);
}

void Game::CreateBricks(int level) {
    bricks.clear();
    float brickWidth = 80;
    float brickHeight = 25;
    float startX = 50;
    float startY = 80;
    float gap = 10;
    
    Color colors[] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };
    int health[] = { 1, 1, 2, 2, 3, 3 };
    int points[] = { 10, 20, 30, 40, 50, 60 };
    
    for (int row = 0; row< 5; row++) {
        for (int col = 0; col < 9; col++) {
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
            lives = 3;
            level = 1;
            ballSpeed = 3;
            ball = Ball((Vector2){400, 300}, (Vector2){ballSpeed, ballSpeed}, 10, RED);
            CreateBricks(level);
        }
    }
    
    else if (currentState == PLAYING) {
        if (IsKeyPressed(KEY_P)) {
            currentState = PAUSED;
        }
        
        ball.Move();
        
        if (ball.BounceEdge(800, 600)) {
            if (ball.GetPosition().y + ball.GetRadius() >= 600) {
                lives--;
                if (lives<= 0) {
                    currentState = GAME_OVER;
                } else {
                    ball = Ball((Vector2){400, 300}, (Vector2){ballSpeed, ballSpeed}, 10, RED);
                }
            }
        }

        if (ball.CheckBrickCollision(bricks)) {
            score += ball.GetScoreValue();
        }
        
        if (IsKeyDown(KEY_LEFT)) paddle.MoveLeft(8);
        if (IsKeyDown(KEY_RIGHT)) paddle.MoveRight(8);

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
            ball = Ball((Vector2){400, 300}, (Vector2){ballSpeed, ballSpeed}, 10, RED);
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
            DrawRectangle(0, 0, 800, 600, Fade(BLACK, 0.5f));
            DrawText("PAUSED", 320, 250, 40, WHITE);
            DrawText("Press P to Resume", 300, 300, 20, WHITE);
        }
    }
    
    else if (currentState == GAME_OVER) {
        DrawText("GAME OVER!", 250, 200, 60, RED);
        DrawText(("Final Score: " + std::to_string(score)).c_str(), 280, 300, 30, GRAY);
        DrawText("Press ENTER to Restart", 250, 350, 30, GRAY);
    }
    
    else if (currentState == VICTORY) {
        DrawText("YOU WIN!", 260, 200, 60, GREEN);
        DrawText(("Final Score: " + std::to_string(score)).c_str(), 280, 300, 30, GRAY);
        DrawText("Press ENTER to Menu", 260, 350, 30, GRAY);
    }

    EndDrawing();
}

void Game::Shutdown() {
    bricks.clear();
}