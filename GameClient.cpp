#include "GameClient.h"
#include "NetworkMessage.h"
#include <iostream>

GameClient::GameClient() : Game(), connected(false) {
    gameMode = TWO_PLAYER_CLIENT;
    currentState = MENU;
}

GameClient::~GameClient() {
    network.disconnect();
}

void GameClient::Init() {
    Game::Init();
    if (network.initClient("127.0.0.1", 12345)) {
        connected = true;
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
        paddle = Paddle(350, windowHeight - 40, paddleWidth, paddleHeight);
        paddleTop = Paddle(350, 30, paddleWidth, paddleHeight);
        localPaddle = &paddleTop;
        remotePaddle = &paddle;
    }
}

void GameClient::Update() {
    if (connected) {
        handleServerPackets();

        if (currentState == PLAYING) {
            if (IsKeyDown(KEY_LEFT)) localPaddle->MoveLeft(paddleSpeed);
            if (IsKeyDown(KEY_RIGHT)) localPaddle->MoveRight(paddleSpeed);
            localPaddle->Update(GetFrameTime());

            sendPaddlePosition();
        }
    }
}

void GameClient::Draw() {
    Game::Draw();

    if (currentState == PLAYING && gameMode == TWO_PLAYER_CLIENT) {
        DrawText(network.isConnected() ? "Connected" : "Connecting...",
                 windowWidth / 2 - 60, 50, 20, network.isConnected() ? GREEN : ORANGE);
    }
}

bool GameClient::ShouldClose() const {
    return Game::ShouldClose();
}

void GameClient::Close() {
    network.disconnect();
    Game::Close();
}

void GameClient::handleServerPackets() {
    network.pollEvents();

    auto packets = network.getPackets();
    for (auto& packet : packets) {
        if (packet.type == "FULLSTATE") {
            try {
                const char* buffer = packet.jsonData.c_str() + 9;

                GameStateHeader header = Serializer::DeserializeHeader(buffer);

                score = header.score;
                lives = header.lives;
                int newLevel = header.level;
                if (newLevel != level) {
                    level = newLevel;
                    CreateBricks(level);
                }
                ballSpeed = header.ballSpeed;
                slowBallEffectTime = header.slowBallEffectTime;

                PaddleData paddleBottomData = Serializer::DeserializePaddle(buffer);
                // PaddleData paddleTopData = Serializer::DeserializePaddle(buffer);
                paddle.MoveTo(paddleBottomData.x, paddleBottomData.y);
                paddle.SetWidth(paddleBottomData.width);
                // paddleTop.MoveTo(paddleTopData.x, paddleTopData.y);
                // paddleTop.SetWidth(paddleTopData.width);

                balls.clear();
                for (int i = 0; i < header.ballCount; i++) {
                    BallData ballData = Serializer::DeserializeBall(buffer);
                    balls.emplace_back(
                        Vector2{ballData.x, ballData.y},
                        Vector2{ballData.vx, ballData.vy},
                        ballRadius, RED);
                }

                std::vector<BrickData> brickDataList;
                for (int i = 0; i < header.brickCount; i++) {
                    BrickData brickData = Serializer::DeserializeBrick(buffer);
                    brickDataList.push_back(brickData);
                }

                CreateBricks(level);

                int idx = 0;
                for (auto& brickData : brickDataList) {
                    if (idx < (int)bricks.size()) {
                        if (!brickData.active && bricks[idx].IsActive()) {
                            bricks[idx].SetActive(false);
                        }
                        bricks[idx].SetHealth(brickData.health);
                        idx++;
                    }
                }

                powerUps.clear();
                for (int i = 0; i < header.powerUpCount; i++) {
                    PowerUpData puData = Serializer::DeserializePowerUp(buffer);
                    PowerUp pu{puData.x, puData.y, static_cast<PowerUpType>(puData.type), *this};
                    pu.active = puData.active;
                    powerUps.push_back(pu);
                }

                particles.clear();
                for (int i = 0; i < header.particleCount; i++) {
                    ParticleData pData = Serializer::DeserializeParticle(buffer);
                    Particle p;
                    p.pos = {pData.x, pData.y};
                    p.vel = {pData.vx, pData.vy};
                    p.life = pData.life;
                    p.color = RED;
                    particles.push_back(p);
                }

            } catch (std::exception& e) {
                std::cout << "Error parsing FULLSTATE: " << e.what() << std::endl;
            } catch (...) {
                std::cout << "Unknown error parsing FULLSTATE" << std::endl;
            }
        }
    }
    network.clearPackets();
}

void GameClient::sendPaddlePosition() {
    if (network.isConnected() && localPaddle) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "CLIENT_PADDLE:%.2f,%.2f",
                 localPaddle->GetRect().x, localPaddle->GetRect().y);
        network.sendPacket(buffer);
    }
}