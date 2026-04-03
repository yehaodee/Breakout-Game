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

void CreateBricks(std::vector<Brick>& bricks, int level) {
    bricks.clear();
    float brickWidth = 80;
    float brickHeight = 25;
    float startX = 50;
    float startY = 80;
    float gap = 10;
    
    // 定义不同类型砖块的颜色、生命值和分数
    Color colors[] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };
    int health[] = { 1, 1, 2, 2, 3, 3 };
    int points[] = { 10, 20, 30, 40, 50, 60 };
    
    // 创建5行砖块
    for (int row = 0; row< 5; row++) {
        for (int col = 0; col < 9; col++) {
            float x = startX + col * (brickWidth + gap);
            float y = startY + row * (brickHeight + gap);
            int brickType = (row + level) % 6;
            bricks.emplace_back(x, y, brickWidth, brickHeight, colors[brickType], health[brickType], points[brickType]);
        }
    }
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Breakout Game - Enhanced");

    GameState currentState = MENU;
    
    // 游戏对象
    Ball ball((Vector2){400, 300}, (Vector2){3, 3}, 10, RED);
    Paddle paddle(350, 550, 100, 20);
    
    // 游戏数据
    int score = 0;
    int lives = 3;
    int level = 1;
    float ballSpeed = 3.0f;
    
    std::vector<Brick> bricks;
    CreateBricks(bricks, level);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // 菜单状态
        if (currentState == MENU) {
            if (IsKeyPressed(KEY_ENTER)) {
                currentState = PLAYING;
                score = 0;
                lives = 3;
                level = 1;
                ballSpeed = 3;
                ball = Ball((Vector2){400, 300}, (Vector2){ballSpeed, ballSpeed}, 10, RED);
                CreateBricks(bricks, level);
            }
        }
        
        // 游戏进行中
        else if (currentState == PLAYING) {
            // 暂停游戏
            if (IsKeyPressed(KEY_P)) {
                currentState = PAUSED;
            }
            
            // 更新球的位置
            ball.Move();
            
            // 边界碰撞检测
            if (ball.BounceEdge(screenWidth, screenHeight)) {
                // 球掉落到底部，减少生命值
                if (ball.GetPosition().y + ball.GetRadius() >= screenHeight) {
                    lives--;
                    if (lives<= 0) {
                        currentState = GAME_OVER;
                    } else {
                        ball = Ball((Vector2){400, 300}, (Vector2){ballSpeed, ballSpeed}, 10, RED);
                    }
                }
            }

            // 球与砖块碰撞检测
            if (ball.CheckBrickCollision(bricks)) {
                score += ball.GetScoreValue();
            }
            
            // 板移动
            if (IsKeyDown(KEY_LEFT)) paddle.MoveLeft(8);
            if (IsKeyDown(KEY_RIGHT)) paddle.MoveRight(8);

            // 砖块碰撞检测
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
            
            // 所有砖块都被摧毁，进入下一关
            if (allBricksDestroyed) {
                level++;
                ballSpeed += 0.5;
                ball = Ball((Vector2){400, 300}, (Vector2){ballSpeed, ballSpeed}, 10, RED);
                CreateBricks(bricks, level);
                
                // 如果是第5关，游戏胜利
                if (level >5) {
                    currentState = VICTORY;
                }
            }

            // Paddle 碰撞检测
            ball.CheckPaddleCollision(paddle.GetRect());
        }
        
        // 暂停状态
        else if (currentState == PAUSED) {
            if (IsKeyPressed(KEY_P)) {
                currentState = PLAYING;
            }
        }
        
        // 游戏结束
        else if (currentState == GAME_OVER) {
            if (IsKeyPressed(KEY_ENTER)) {
                currentState = MENU;
            }
        }
        
        // 游戏胜利
        else if (currentState == VICTORY) {
            if (IsKeyPressed(KEY_ENTER)) {
                currentState = MENU;
            }
        }

        // 绘制
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
            
            // Draw game UI
            DrawText(("Score: " + std::to_string(score)).c_str(), 20, 20, 20, DARKGRAY);
            DrawText(("Lives: " + std::to_string(lives)).c_str(), 150, 20, 20, DARKGRAY);
            DrawText(("Level: " + std::to_string(level)).c_str(), 300, 20, 20, DARKGRAY);
            
            if (currentState == PAUSED) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
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

    CloseWindow();
    return 0;
}