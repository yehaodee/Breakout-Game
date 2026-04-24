#include "Game.h"
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iostream>

using json = nlohmann::json;

Game::Game() // 这些初始化参数相当于默认值
    : paddle(350, 550, 100, 20),
      paddleTop(350, 30, 100, 20),
      score(0),
      lives(3),
      level(1),
      currentState(MENU),
      gameMode(SINGLE_PLAYER),
      slowBallEffectTime(0.0f),
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
      timeMultiplierDecay(0.05f) {
    LoadConfig("config.json");
    balls.emplace_back((Vector2){400, 300}, (Vector2){3, 3}, 10, RED);
    localPaddle = &paddle;
    remotePaddle = &paddleTop;
}

Game::~Game() {
}

void Game::Init() {
    InitWindow(windowWidth, windowHeight, windowTitle.c_str());
    SetTargetFPS(60);
}

void Game::CreateBricks(int level) {
    bricks.clear();
    float startX = 50;
    float startY = 80;
    float gap = 10;

    Color colors[] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };
    int health[] = { 1, 1, 1, 1, 2, 2 };
    int points[] = { 10, 20, 30, 40, 50, 60 };

    for (int row = 0; row < brickRows; row++) {
        for (int col = 0; col < brickCols; col++) {
            float x = startX + col * (brickWidth + gap);
            float y = startY + row * (brickHeight + gap);
            int brickType = (row + level) % 6;
            bricks.emplace_back(x, y, brickWidth, brickHeight, colors[brickType], health[brickType], points[brickType]);
        }
    }
}

void Game::Update() {
    
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (currentState == MENU) {
        DrawText("BREAKOUT GAME", 180, 150, 60, DARKBLUE);
        DrawText("Press ENTER to Start Game", 180, 280, 30, GRAY);
        DrawText("Press P to Pause", 280, 450, 20, LIGHTGRAY);
    }

    else if (currentState == PLAYING || currentState == PAUSED) {
        for (auto& ball : balls) {
            ball.Draw();
        }
        paddle.Draw();
        if (gameMode == TWO_PLAYER_HOST || gameMode == TWO_PLAYER_CLIENT) {
            paddleTop.Draw();
        }
        for (auto& brick : bricks) brick.Draw();

        for (auto& powerUp : powerUps) {
            powerUp.Draw();
        }

        for (auto& particle : particles) {
            particle.Draw();
        }

        std::string modeStr = "SINGLE";
        if (gameMode == TWO_PLAYER_HOST) {
            modeStr = "HOST";
        } else if (gameMode == TWO_PLAYER_CLIENT) {
            modeStr = "CLIENT";
        }
        DrawText(("Mode: " + modeStr).c_str(), 20, 20, 20, DARKBLUE);
        DrawText(("Score: " + std::to_string(score)).c_str(), 150, 20, 20, DARKGRAY);
        DrawText(("Lives: " + std::to_string(lives)).c_str(), 300, 20, 20, DARKGRAY);
        DrawText(("Level: " + std::to_string(level)).c_str(), 450, 20, 20, DARKGRAY);

        if (currentState == PAUSED) {
            DrawRectangle(0, 0, windowWidth, windowHeight, Fade(BLACK, 0.5f));
            DrawText("PAUSED", windowWidth / 2 - 100, windowHeight / 2 - 20, 40, WHITE);
            DrawText("Press P to Resume", windowWidth / 2 - 100, windowHeight / 2 + 20, 20, WHITE);
        }
    }

    else if (currentState == GAME_OVER) {
        DrawText("GAME OVER!", windowWidth / 2 - 150, 200, 60, RED);
        DrawText(("Final Score: " + std::to_string(score)).c_str(), windowWidth / 2 - 100, 300, 30, GRAY);
        DrawText("Press ENTER for Menu", windowWidth / 2 - 100, 350, 30, GRAY);
    }

    else if (currentState == VICTORY) {
        DrawText("YOU WIN!", windowWidth / 2 - 120, 200, 60, GREEN);
        DrawText(("Final Score: " + std::to_string(score)).c_str(), windowWidth / 2 - 100, 300, 30, GRAY);
        DrawText("Press ENTER for Menu", windowWidth / 2 - 120, 350, 30, GRAY);
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

    powerUpConfig[0].extraWidth = config["powerups"]["paddle_extend"]["extra_width"];
    powerUpConfig[0].duration = config["powerups"]["paddle_extend"]["duration"];
    powerUpConfig[0].dropRate = config["powerups"]["paddle_extend"]["drop_rate"];

    powerUpConfig[1].extraBalls = config["powerups"]["multi_ball"]["extra_balls"];
    powerUpConfig[1].duration = config["powerups"]["multi_ball"]["duration"];
    powerUpConfig[1].dropRate = config["powerups"]["multi_ball"]["drop_rate"];

    powerUpConfig[2].speedFactor = config["powerups"]["slow_ball"]["speed_factor"];
    powerUpConfig[2].duration = config["powerups"]["slow_ball"]["duration"];
    powerUpConfig[2].dropRate = config["powerups"]["slow_ball"]["drop_rate"];
}

bool Game::ShouldClose() const {
    return WindowShouldClose();
}

void Game::Close() {
    Shutdown();
}

void Game::Shutdown() {
    bricks.clear();
}