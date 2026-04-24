#include "GameServer.h"
#include "NetworkMessage.h"
#include <cmath>

GameServer::GameServer() : Game() {
    gameMode = TWO_PLAYER_HOST;
    currentState = MENU;
}

GameServer::~GameServer() {
    network.disconnect();
}

void GameServer::Init() {
    Game::Init();
    if (network.initServer(12345)) {
        currentState = MENU;
        score = 0;
        lives = initialLives;
        level = 1;
        ballSpeed = 3;
        balls.clear();
        balls.emplace_back((Vector2){windowWidth / 2.0f, windowHeight / 2.0f}, (Vector2){ballSpeed, ballSpeed}, ballRadius, RED);
        powerUps.clear();
        particles.clear();
        CreateBricks(level);
        paddle = Paddle(350, windowHeight - 40, paddleWidth, paddleHeight);
        paddleTop = Paddle(350, 30, paddleWidth, paddleHeight);
        localPaddle = &paddleTop;
        remotePaddle = &paddle;
    }
}

void GameServer::Update() {
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
            localPaddle = &paddle;
            remotePaddle = &paddleTop;
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

    if (currentState == PLAYING && gameMode == TWO_PLAYER_HOST) {
        handleClientPackets();
        sendGameState();
    }
}

void GameServer::Draw() {
    Game::Draw();
    
    if (currentState == PLAYING && gameMode == TWO_PLAYER_HOST) {
        DrawText(network.isConnected() ? "Client Connected" : "Waiting for client...",
                 windowWidth / 2 - 80, 50, 20, network.isConnected() ? GREEN : ORANGE);
    }
}

bool GameServer::ShouldClose() const {
    return Game::ShouldClose();
}

void GameServer::Close() {
    network.disconnect();
    Game::Close();
}

void GameServer::handleClientPackets() {
    network.pollEvents();

    auto packets = network.getPackets();
    for (auto& packet : packets) {
        if (packet.type == "CLIENT_PADDLE") {
            if (remotePaddle) {
                remotePaddle->MoveTo(packet.data[0], packet.data[1]);
            }
        }
    }
    network.clearPackets();
}

void GameServer::sendGameState() {
    if (network.isConnected()) {
        size_t bufferSize = Serializer::CalculateBufferSize(
            balls.size(), bricks.size(), powerUps.size(), particles.size());
        std::vector<char> buffer(bufferSize + 64);
        char* ptr = buffer.data();

        GameStateHeader header;
        header.score = score;
        header.lives = lives;
        header.level = level;
        header.ballSpeed = ballSpeed;
        header.slowBallEffectTime = slowBallEffectTime;
        header.ballCount = balls.size();
        header.paddleBottomCount = 1;
        header.paddleTopCount = 1;
        header.brickCount = bricks.size();
        header.powerUpCount = powerUps.size();
        header.particleCount = particles.size();
        Serializer::SerializeHeader(header, ptr);

        PaddleData paddleBottomData;
        paddleBottomData.x = paddle.GetRect().x;
        paddleBottomData.y = paddle.GetRect().y;
        paddleBottomData.width = paddle.GetRect().width;
        Serializer::SerializePaddle(paddleBottomData, ptr);

        // PaddleData paddleTopData;
        // paddleTopData.x = paddleTop.GetRect().x;
        // paddleTopData.y = paddleTop.GetRect().y;
        // paddleTopData.width = paddleTop.GetRect().width;
        // Serializer::SerializePaddle(paddleTopData, ptr);

        for (auto& ball : balls) {
            BallData ballData;
            ballData.x = ball.GetPosition().x;
            ballData.y = ball.GetPosition().y;
            ballData.vx = ball.GetVelocity().x;
            ballData.vy = ball.GetVelocity().y;
            ballData.radius = ballRadius;
            ballData.color = 0xFF0000FF;
            Serializer::SerializeBall(ballData, ptr);
        }

        for (auto& brick : bricks) {
            BrickData brickData;
            brickData.x = brick.GetRect().x;
            brickData.y = brick.GetRect().y;
            brickData.width = brick.GetRect().width;
            brickData.height = brick.GetRect().height;
            brickData.active = brick.IsActive();
            brickData.health = brick.GetHealth();
            Serializer::SerializeBrick(brickData, ptr);
        }

        for (auto& pu : powerUps) {
            PowerUpData puData;
            puData.x = pu.position.x;
            puData.y = pu.position.y;
            puData.active = pu.active;
            puData.type = static_cast<int>(pu.type);
            Serializer::SerializePowerUp(puData, ptr);
        }

        for (auto& p : particles) {
            ParticleData pData;
            pData.x = p.pos.x;
            pData.y = p.pos.y;
            pData.vx = p.vel.x;
            pData.vy = p.vel.y;
            pData.life = p.life;
            pData.color = 0xFFFFFFFF;
            Serializer::SerializeParticle(pData, ptr);
        }

        std::string data = "FULLSTATE";
        data.append(buffer.data(), buffer.size());
        network.sendPacket(data);
    }
}