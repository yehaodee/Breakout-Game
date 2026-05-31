#include "Game.h"
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <unordered_map>

using json = nlohmann::json;

Game::Game()
    : paddle(350, 550, 100, 20),
      paddleTop(350, 30, 100, 20),
      score(0),
      lives(3),
      level(1),
      levelName("Level 1"),
      currentState(MENU),
      gameMode(SINGLE_PLAYER),
      isLoading(false),
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
      timeMultiplierDecay(0.05f),
      cellWidth(800.0f / GRID_WIDTH),
      cellHeight(600.0f / GRID_HEIGHT),
      hasBackgroundImage(false)
      {
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
    if (!LoadLevel(level)) {
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
        levelName = "Level " + std::to_string(level);
    }
}

bool Game::LoadLevel(int levelNum) {
    std::ifstream file("levels.json");
    if (!file.is_open()) {
        std::cout << "Failed to open levels.json" << std::endl;
        return false;
    }

    try {
        json levelsData = json::parse(file);
        file.close();

        if (!levelsData.contains("levels") || levelsData["levels"].size() < levelNum) {
            std::cout << "Level " << levelNum << " not found in levels.json" << std::endl;
            return false;
        }

        json& levelData = levelsData["levels"][levelNum - 1];
        levelName = levelData.value("name", "Level " + std::to_string(levelNum));

        bricks.clear();

        if (!levelData.contains("layout") || !levelData.contains("brick_types")) {
            return false;
        }

        auto& layout = levelData["layout"];
        auto& brickTypes = levelData["brick_types"];

        int rows = layout.size();
        if (rows == 0) return false;
        int cols = layout[0].size();

        float startX = (windowWidth - cols * (brickWidth + 10)) / 2.0f + 5;
        float startY = 80;
        float gap = 10;

        for (int row = 0; row < rows && row < 10; row++) {
            for (int col = 0; col < cols && col < 12; col++) {
                int typeKey = layout[row][col].get<int>();
                std::string typeStr = std::to_string(typeKey);

                if (brickTypes.contains(typeStr)) {
                    auto& brickInfo = brickTypes[typeStr];
                    int health = brickInfo.value("health", 1);
                    
                    if (health <= 0) continue;

                    auto& colorArr = brickInfo["color"];
                    Color brickColor = {
                        static_cast<unsigned char>(colorArr[0].get<int>()),
                        static_cast<unsigned char>(colorArr[1].get<int>()),
                        static_cast<unsigned char>(colorArr[2].get<int>()),
                        255
                    };

                    int points = brickInfo.value("points", 10);
                    float x = startX + col * (brickWidth + gap);
                    float y = startY + row * (brickHeight + gap);

                    bricks.emplace_back(x, y, brickWidth, brickHeight, brickColor, health, points);
                }
            }
        }

        std::cout << "Loaded level: " << levelName << " with " << bricks.size() << " bricks" << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cout << "Error parsing levels.json: " << e.what() << std::endl;
        return false;
    }
}

void Game::Update() {
    
}

void Game::Draw() {
    BeginDrawing();
    
    if (hasBackgroundImage) {
        DrawTextureEx(backgroundTexture, (Vector2){0, 0}, 0.0f, 1.0f, (Color){255, 255, 255, 80});
    } else {
        ClearBackground(RAYWHITE);
    }

    if (currentState == MENU) {
        DrawText("BREAKOUT GAME", 180, 150, 60, DARKBLUE);
        DrawText("Press ENTER to Start Game", 180, 280, 30, GRAY);
        if (HasSaveFile("save.json")) {
            DrawText("Press R to Resume Saved Game", 180, 330, 30, GREEN);
        }
        DrawText("Press P to Pause", 280, 450, 20, LIGHTGRAY);
        DrawText("Press S to Save Game", 280, 480, 20, LIGHTGRAY);
    }

    else if (currentState == PLAYING || currentState == PAUSED || currentState == LOADING) {
        for (auto& ball : balls) {
            ball.Draw();
        }
        paddle.Draw();
        if (gameMode == TWO_PLAYER_HOST || gameMode == TWO_PLAYER_CLIENT) {
            paddleTop.Draw();
        }
        DrawBricksBatch();

        for (auto& powerUp : powerUps) {
            powerUp.Draw();
        }

        particlePool.Draw();

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
        DrawText(levelName.c_str(), 550, 20, 18, DARKGRAY);

        {
            std::lock_guard<std::mutex> lock(loadingMutex);
            if (isLoading) {
                DrawRectangle(0, windowHeight/2 - 30, windowWidth, 60, Fade(BLACK, 0.5f));
                DrawText("Loading...", windowWidth/2 - 50, windowHeight/2 - 10, 30, WHITE);
            }
        }

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

void Game::SimulateTextureLoading() {
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void Game::ResetGame() {
    score = 0;
    lives = initialLives;
    level = 1;
    ballSpeed = 3;
    balls.clear();
    balls.emplace_back((Vector2){windowWidth / 2.0f, windowHeight / 2.0f}, (Vector2){ballSpeed, ballSpeed}, ballRadius, RED);
    powerUps.clear();
    particlePool.Clear();
    CreateBricks(level);
}

void Game::UpdatePlayingState() {
    if (IsKeyPressed(KEY_P)) {
        currentState = PAUSED;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(loadingMutex);
        if (IsKeyPressed(KEY_L) && !isLoading) {
            isLoading = true;
            loadingFuture = std::async(std::launch::async, [this]() {
                SimulateTextureLoading();
            });
        }
    }

    {
        std::lock_guard<std::mutex> lock(loadingMutex);
        if (isLoading && loadingFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            loadingFuture.get();
            isLoading = false;
        }
    }

    for (size_t i = 0; i < balls.size(); i++) {
        Ball& ball = balls[i];
        ball.Move();

        if (ball.BounceEdge(windowWidth, windowHeight)) {
            if (ball.GetPosition().y + ball.GetRadius() >= windowHeight) {
                balls.erase(balls.begin() + i);
                i--;

                if (balls.empty()) {
                    lives--;
                    if (lives <= 0) {
                        currentState = GAME_OVER;
                    } else {
                        balls.emplace_back((Vector2){windowWidth / 2.0f, windowHeight / 2.0f}, (Vector2){ballSpeed, ballSpeed}, ballRadius, RED);
                    }
                }
            }
        }
    }

    if (IsKeyDown(KEY_LEFT)) localPaddle->MoveLeft(paddleSpeed);
    if (IsKeyDown(KEY_RIGHT)) localPaddle->MoveRight(paddleSpeed);
    localPaddle->Update(GetFrameTime());

    BuildGrid();
    
    bool allBricksDestroyed = true;
    for (auto& brick : bricks) {
        if (brick.IsActive()) {
            allBricksDestroyed = false;
            break;
        }
    }
    
    if (!allBricksDestroyed) {
        for (auto& ball : balls) {
            Vector2 ballPos = ball.GetPosition();
            int ballGx = static_cast<int>(ballPos.x / cellWidth);
            int ballGy = static_cast<int>(ballPos.y / cellHeight);
            
            // 遍历球所在网格及其相邻网格（3x3区域）
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int gx = ballGx + dx;
                    int gy = ballGy + dy;
                    
                    // 确保网格索引有效
                    if (gx < 0 || gx >= GRID_WIDTH || gy < 0 || gy >= GRID_HEIGHT) {
                        continue;
                    }
                    
                    // 只检测当前网格内的砖块
                    for (Brick* brick : grid[gx][gy]) {
                        if (brick->IsActive() && brick->CheckCollision(ball)) {
                            ball.ReverseY();
                            score += brick->GetPoints();
                            
                            if (!brick->IsActive()) {
                                for (int i = 0; i < 10; i++) {
                                    Rectangle rect = brick->GetRect();
                                    Vector2 pos = { rect.x + rand() % (int)rect.width,
                                                    rect.y + rand() % (int)rect.height };
                                    Vector2 vel = { (rand() % 100 - 50) / 10.0f, (rand() % 100 - 50) / 10.0f };
                                    particlePool.Create(pos, vel, brick->GetColor(), 0.5f);
                                }
                                
                                PowerUpType type = static_cast<PowerUpType>(rand() % 3);
                                float dropRate = powerUpConfig[static_cast<int>(type)].dropRate * 100;
                                if (rand() % 100 < dropRate) {
                                    Rectangle rect = brick->GetRect();
                                    powerUps.emplace_back(rect.x + rect.width / 2,
                                                          rect.y, type, *this);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (allBricksDestroyed) {
        level++;
        if (level > MAX_LEVELS) {
            currentState = VICTORY;
        } else {
            ballSpeed += 0.5;
            balls.clear();
            balls.emplace_back((Vector2){windowWidth / 2.0f, windowHeight / 2.0f},
                               (Vector2){ballSpeed, ballSpeed}, ballRadius, RED);
            CreateBricks(level);
        }
    }

    for (auto& powerUp : powerUps) {
        powerUp.Update(GetFrameTime());

        if (powerUp.active && CheckCollisionCircleRec(powerUp.position, 10, paddle.GetRect())) {
            powerUp.Apply(*this, paddle);
            powerUp.active = false;
        }
    }

    for (auto& ball : balls) {
        ball.CheckPaddleCollision(paddle.GetRect());
    }

    if (slowBallEffectTime > 0) {
        slowBallEffectTime -= GetFrameTime();
        if (slowBallEffectTime <= 0) {
            for (auto& ball : balls) {
                Vector2 vel = ball.GetVelocity();
                float speed = sqrt(vel.x * vel.x + vel.y * vel.y);
                float normalizedSpeed = speed / 0.7f;
                float angle = atan2(vel.y, vel.x);
                ball.SetVelocity((Vector2){cos(angle) * normalizedSpeed, sin(angle) * normalizedSpeed});
            }
        }
    }

    particlePool.Update(GetFrameTime());
}

void Game::ClearGrid() {
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            grid[x][y].clear();
        }
    }
}

void Game::BuildGrid() {
    ClearGrid();
    for (auto& brick : bricks) {
        if (brick.IsActive()) {
            Rectangle rect = brick.GetRect();
            int gx = static_cast<int>(rect.x / cellWidth);
            int gy = static_cast<int>(rect.y / cellHeight);
            
            // 确保网格索引在有效范围内
            gx = std::max(0, std::min(GRID_WIDTH - 1, gx));
            gy = std::max(0, std::min(GRID_HEIGHT - 1, gy));
            
            grid[gx][gy].push_back(&brick);
        }
    }
}

void Game::DrawBricksBatch() {
    std::unordered_map<unsigned int, std::vector<const Brick*>> bricksByColor;
    
    for (const auto& brick : bricks) {
        if (brick.IsActive()) {
            Color col = brick.GetColor();
            unsigned int colorKey = 
                (static_cast<unsigned int>(col.r) << 24) |
                (static_cast<unsigned int>(col.g) << 16) |
                (static_cast<unsigned int>(col.b) << 8) |
                static_cast<unsigned int>(col.a);
            bricksByColor[colorKey].push_back(&brick);
        }
    }
    
    for (const auto& [colorKey, brickList] : bricksByColor) {
        Color col = {
            static_cast<unsigned char>((colorKey >> 24) & 0xFF),
            static_cast<unsigned char>((colorKey >> 16) & 0xFF),
            static_cast<unsigned char>((colorKey >> 8) & 0xFF),
            static_cast<unsigned char>(colorKey & 0xFF)
        };
        
        for (const Brick* brick : brickList) {
            DrawRectangleRec(brick->GetRect(), col);
        }
    }
    
    for (const auto& brick : bricks) {
        if (brick.IsActive() && brick.GetHealth() > 1) {
            DrawRectangleLinesEx(brick.GetRect(), 2, DARKGRAY);
        }
    }
}

void Game::Shutdown() {
    bricks.clear();
    if (hasBackgroundImage) {
        UnloadTexture(backgroundTexture);
        hasBackgroundImage = false;
    }
}

bool Game::SaveGame(const std::string& path) {
    try {
        json saveData;
        
        saveData["level"] = level;
        saveData["score"] = score;
        saveData["lives"] = lives;
        saveData["ballSpeed"] = ballSpeed;
        
        json ballsArray = json::array();
        for (const auto& ball : balls) {
            json ballData;
            Vector2 pos = ball.GetPosition();
            Vector2 vel = ball.GetVelocity();
            ballData["x"] = pos.x;
            ballData["y"] = pos.y;
            ballData["vx"] = vel.x;
            ballData["vy"] = vel.y;
            ballsArray.push_back(ballData);
        }
        saveData["balls"] = ballsArray;
        
        json paddleData;
        Rectangle paddleRect = paddle.GetRect();
        paddleData["x"] = paddleRect.x;
        paddleData["y"] = paddleRect.y;
        paddleData["width"] = paddleRect.width;
        saveData["paddle"] = paddleData;
        
        json bricksArray = json::array();
        for (const auto& brick : bricks) {
            json brickData;
            Rectangle rect = brick.GetRect();
            Color col = brick.GetColor();
            brickData["x"] = rect.x;
            brickData["y"] = rect.y;
            brickData["width"] = rect.width;
            brickData["height"] = rect.height;
            brickData["r"] = col.r;
            brickData["g"] = col.g;
            brickData["b"] = col.b;
            brickData["a"] = col.a;
            brickData["health"] = brick.GetHealth();
            brickData["points"] = brick.GetPoints();
            brickData["active"] = brick.IsActive();
            bricksArray.push_back(brickData);
        }
        saveData["bricks"] = bricksArray;
        
        std::ofstream file(path);
        file << std::setw(4) << saveData << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Save game failed: " << e.what() << std::endl;
        return false;
    }
}

bool Game::LoadGame(const std::string& path) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            return false;
        }
        
        json saveData = json::parse(file);
        
        level = saveData["level"];
        score = saveData["score"];
        lives = saveData["lives"];
        ballSpeed = saveData["ballSpeed"];
        
        balls.clear();
        for (const auto& ballData : saveData["balls"]) {
            Vector2 pos = {ballData["x"], ballData["y"]};
            Vector2 vel = {ballData["vx"], ballData["vy"]};
            balls.emplace_back(pos, vel, ballRadius, RED);
        }
        
        paddle.MoveTo(saveData["paddle"]["x"], saveData["paddle"]["y"]);
        paddle.SetWidth(saveData["paddle"]["width"]);
        
        bricks.clear();
        for (const auto& brickData : saveData["bricks"]) {
            Color col = {
                static_cast<unsigned char>(brickData["r"]),
                static_cast<unsigned char>(brickData["g"]),
                static_cast<unsigned char>(brickData["b"]),
                static_cast<unsigned char>(brickData["a"])
            };
            Brick brick(
                brickData["x"],
                brickData["y"],
                brickData["width"],
                brickData["height"],
                col,
                brickData["health"],
                brickData["points"]
            );
            brick.SetActive(brickData["active"]);
            bricks.push_back(brick);
        }
        
        powerUps.clear();
        particlePool.Clear();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Load game failed: " << e.what() << std::endl;
        return false;
    }
}

bool Game::HasSaveFile(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

void Game::SetBackgroundImage(const std::string& path) {
    if (hasBackgroundImage) {
        UnloadTexture(backgroundTexture);
    }
    backgroundImagePath = path;
    backgroundTexture = ::LoadTexture(path.c_str());
    if (backgroundTexture.id != 0) {
        hasBackgroundImage = true;
    }
}