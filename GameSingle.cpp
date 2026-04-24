#include "GameSingle.h"
#include <cmath> 

GameSingle::GameSingle() : Game() {
    gameMode = SINGLE_PLAYER;
    currentState = MENU;
}

GameSingle::~GameSingle() {
}

void GameSingle::Init() {
    Game::Init();
}

void GameSingle::Update() {
    switch (currentState) {
    case MENU: {
        if (IsKeyPressed(KEY_ENTER)) {
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
        }
        break;
    }
    case PLAYING: {
        if (IsKeyPressed(KEY_P)) {
            currentState = PAUSED;
            break;
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

        for (size_t i = 0; i < particles.size(); i++) {
            particles[i].Update(GetFrameTime());
            if (particles[i].life <= 0) {
                particles.erase(particles.begin() + i);
                i--;
            }
        }
        break;
    }
    case PAUSED: {
        if (IsKeyPressed(KEY_P)) {
            currentState = PLAYING;
        }
        break;
    }
    case GAME_OVER:
    case VICTORY: {
        if (IsKeyPressed(KEY_ENTER)) {
            currentState = MENU;
        }
        break;
    }
    }
}
