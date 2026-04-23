#include "Game.h"
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

Game::Game()
    : paddle(350, 550, 100, 20),
      paddleTop(350, 30, 100, 20),
      score(0),
      lives(3),
      level(1),
      currentState(MENU),
      gameMode(SINGLE_PLAYER),
      gameTime(0.0f),
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
    network.disconnect();
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
    int health[] = { 1, 1, 2, 2, 3, 3 };
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

void Game::startTwoPlayerHost() {
    network.disconnect();
    if (network.initServer(12345)) {
        gameMode = TWO_PLAYER_HOST;
        currentState = PLAYING;
        score = 0;
        lives = initialLives;
        level = 1;
        ballSpeed = 3;
        balls.clear();
        balls.emplace_back((Vector2){windowWidth / 2.0f, windowHeight / 2.0f}, (Vector2){ballSpeed, ballSpeed}, ballRadius, RED);
        powerUps.clear();
        particles.clear();
        CreateBricks(level);
        localPaddle = &paddle;
        remotePaddle = &paddleTop;
        paddle = Paddle(350, windowHeight - 40, paddleWidth, paddleHeight);
        paddleTop = Paddle(350, 30, paddleWidth, paddleHeight);
    }
}

void Game::startTwoPlayerClient() {
    network.disconnect();
    if (network.initClient("127.0.0.1", 12345)) {
        gameMode = TWO_PLAYER_CLIENT;
        currentState = PLAYING;
        score = 0;
        lives = initialLives;
        level = 1;
        ballSpeed = 3;
        balls.clear();
        balls.emplace_back((Vector2){windowWidth / 2.0f, windowHeight / 2.0f}, (Vector2){ballSpeed, ballSpeed}, ballRadius, RED);
        powerUps.clear();
        particles.clear();
        CreateBricks(level);
        localPaddle = &paddleTop;
        remotePaddle = &paddle;
        paddle = Paddle(350, windowHeight - 40, paddleWidth, paddleHeight);
        paddleTop = Paddle(350, 30, paddleWidth, paddleHeight);
    }
}

void Game::sendGameState() {
    if (gameMode == TWO_PLAYER_HOST && network.isConnected()) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "BALL:%.2f,%.2f,%.2f,%.2f",
                 balls[0].GetPosition().x, balls[0].GetPosition().y,
                 balls[0].GetVelocity().x, balls[0].GetVelocity().y);
        network.sendPacket(buffer);

        snprintf(buffer, sizeof(buffer), "GAMESTATE:%d,%d,%d,%d,%d,%.2f,%.2f",
                 score, lives, level, (int)balls.size(),
                 (int)bricks.size(), ballSpeed, slowBallEffectTime);
        network.sendPacket(buffer);

        for (auto& brick : bricks) {
            snprintf(buffer, sizeof(buffer), "BRICK:%.2f,%.2f,%d",
                     brick.GetRect().x, brick.GetRect().y, brick.IsActive() ? 1 : 0);
            network.sendPacket(buffer);
        }
    }
}

void Game::handleNetworkPackets() {
    network.pollEvents();

    auto packets = network.getPackets();
    for (auto& packet : packets) {
        if (packet.type == "PADDLE") {
            remotePaddle->MoveTo(packet.data[0], packet.data[1]);
        }
    }
    network.clearPackets();
}

void Game::Update() {
    gameTime += GetFrameTime();

    if (currentState == MENU) {
        if (IsKeyPressed(KEY_ENTER)) {
            currentState = PLAYING;
            gameMode = SINGLE_PLAYER;
            score = 0;
            lives = initialLives;
            level = 1;
            ballSpeed = 3;
            balls.clear();
            balls.emplace_back((Vector2){windowWidth / 2.0f, windowHeight / 2.0f}, (Vector2){ballSpeed, ballSpeed}, ballRadius, RED);
            powerUps.clear();
            particles.clear();
            CreateBricks(level);
        }
        else if (IsKeyPressed(KEY_H)) {
            startTwoPlayerHost();
        }
        else if (IsKeyPressed(KEY_K)) {
            startTwoPlayerClient();
        }
    }

    else if (currentState == PLAYING) {
        if (IsKeyPressed(KEY_P)) {
            currentState = PAUSED;
        }

        if (gameMode == TWO_PLAYER_HOST || gameMode == TWO_PLAYER_CLIENT) {
            handleNetworkPackets();
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

        if (gameMode != TWO_PLAYER_CLIENT) {
            if (IsKeyDown(KEY_LEFT)) localPaddle->MoveLeft(paddleSpeed);
            if (IsKeyDown(KEY_RIGHT)) localPaddle->MoveRight(paddleSpeed);
            localPaddle->Update(GetFrameTime());

            char buffer[64];
            snprintf(buffer, sizeof(buffer), "PADDLE:%.2f,%.2f",
                     localPaddle->GetRect().x, localPaddle->GetRect().y);
            network.sendPacket(buffer);
        }

        bool allBricksDestroyed = true;
        for (auto& brick : bricks) {
            if (brick.IsActive()) {
                allBricksDestroyed = false;
                for (auto& ball : balls) {
                    if (brick.CheckCollision(ball)) {
                        ball.ReverseY();
                        score += brick.GetPoints();

                        if (!brick.IsActive()) {
                            for (int i = 0; i < 10; i++) {
                                Particle p;
                                p.pos = { brick.GetRect().x + rand() % (int)brick.GetRect().width,
                                          brick.GetRect().y + rand() % (int)brick.GetRect().height };
                                p.vel = { (rand() % 100 - 50) / 10.0f, (rand() % 100 - 50) / 10.0f };
                                p.color = brick.GetColor();
                                p.life = 0.5f;
                                particles.push_back(p);
                            }

                            PowerUpType type = static_cast<PowerUpType>(rand() % 3);
                            float dropRate = powerUpConfig[static_cast<int>(type)].dropRate * 100;
                            if (rand() % 100 < dropRate) {
                                powerUps.emplace_back(brick.GetRect().x + brick.GetRect().width / 2,
                                                      brick.GetRect().y, type, *this);
                            }
                        }
                    }
                }
            }
        }

        if (allBricksDestroyed) {
            level++;
            ballSpeed += 0.5;
            balls.clear();
            balls.emplace_back((Vector2){windowWidth / 2.0f, windowHeight / 2.0f},
                               (Vector2){ballSpeed, ballSpeed}, ballRadius, RED);
            CreateBricks(level);

            if (level > 5) {
                currentState = VICTORY;
            }
        }

        for (auto& powerUp : powerUps) {
            powerUp.Update(GetFrameTime());

            if (powerUp.active && CheckCollisionCircleRec(powerUp.position, 10, paddle.GetRect())) {
                powerUp.Apply(*this);
                powerUp.active = false;
            }
            if (powerUp.active && CheckCollisionCircleRec(powerUp.position, 10, paddleTop.GetRect())) {
                powerUp.Apply(*this);
                powerUp.active = false;
            }
        }

        for (auto& ball : balls) {
            ball.CheckPaddleCollision(paddle.GetRect());
            ball.CheckPaddleCollision(paddleTop.GetRect());
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

        for (size_t i = 0; i < particles.size(); i++) {
            particles[i].Update(GetFrameTime());
            if (particles[i].life <= 0) {
                particles.erase(particles.begin() + i);
                i--;
            }
        }

        if (gameMode == TWO_PLAYER_HOST) {
            sendGameState();
        }
    }

    else if (currentState == PAUSED) {
        if (IsKeyPressed(KEY_P)) {
            currentState = PLAYING;
        }
    }

    else if (currentState == GAME_OVER || currentState == VICTORY) {
        if (IsKeyPressed(KEY_ENTER)) {
            if (gameMode == TWO_PLAYER_HOST || gameMode == TWO_PLAYER_CLIENT) {
                network.disconnect();
                gameMode = SINGLE_PLAYER;
            }
            currentState = MENU;
        }
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (currentState == MENU) {
        DrawText("BREAKOUT GAME", 180, 150, 60, DARKBLUE);
        DrawText("Press ENTER for Single Player", 180, 280, 30, GRAY);
        DrawText("Press H for Two Player (Host)", 180, 330, 30, GRAY);
        DrawText("Press K for Two Player (Client)", 180, 380, 30, GRAY);
        DrawText("Press P to Pause", 280, 450, 20, LIGHTGRAY);
    }

    else if (currentState == PLAYING || currentState == PAUSED) {
        for (auto& ball : balls) {
            ball.Draw();
        }
        paddle.Draw();
        paddleTop.Draw();
        for (auto& brick : bricks) brick.Draw();

        for (auto& powerUp : powerUps) {
            powerUp.Draw();
        }

        for (auto& particle : particles) {
            particle.Draw();
        }

        std::string modeStr = (gameMode == SINGLE_PLAYER) ? "SINGLE" :
                              (gameMode == TWO_PLAYER_HOST) ? "HOST" : "CLIENT";
        DrawText(("Mode: " + modeStr).c_str(), 20, 20, 20, DARKBLUE);
        DrawText(("Score: " + std::to_string(score)).c_str(), 150, 20, 20, DARKGRAY);
        DrawText(("Lives: " + std::to_string(lives)).c_str(), 300, 20, 20, DARKGRAY);
        DrawText(("Level: " + std::to_string(level)).c_str(), 450, 20, 20, DARKGRAY);

        if (gameMode == TWO_PLAYER_HOST) {
            DrawText(network.isConnected() ? "Client Connected" : "Waiting for client...",
                     windowWidth / 2 - 80, 50, 20, network.isConnected() ? GREEN : ORANGE);
        } else if (gameMode == TWO_PLAYER_CLIENT) {
            DrawText(network.isConnected() ? "Connected" : "Connecting...",
                     windowWidth / 2 - 60, 50, 20, network.isConnected() ? GREEN : ORANGE);
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

void Game::Shutdown() {
    bricks.clear();
    network.disconnect();
}