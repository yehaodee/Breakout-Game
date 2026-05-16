#include "GameServer.h"
#include "NetworkMessage.h"

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
        ResetGame();
        paddle = Paddle(350, windowHeight - 40, paddleWidth, paddleHeight);
        paddleTop = Paddle(350, 30, paddleWidth, paddleHeight);
        localPaddle = &paddle;
        remotePaddle = &paddleTop;
    }
}

void GameServer::Update() {
    switch (currentState) {
    case MENU: {
        if (IsKeyPressed(KEY_ENTER)) {
            ResetGame();
            localPaddle = &paddle;
            remotePaddle = &paddleTop;
            currentState = PLAYING;
        }
        break;
    }
    case PLAYING: {
        UpdatePlayingState();
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
            balls.size(), bricks.size(), powerUps.size(), particlePool.GetActiveCount());
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
        header.particleCount = particlePool.GetActiveCount();
        Serializer::SerializeHeader(header, ptr);

        PaddleData paddleBottomData;
        paddleBottomData.x = paddle.GetRect().x;
        paddleBottomData.y = paddle.GetRect().y;
        paddleBottomData.width = paddle.GetRect().width;
        Serializer::SerializePaddle(paddleBottomData, ptr);

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

        for (int i = 0; i < particlePool.GetMaxParticles(); i++) {
            if (particlePool.IsActive(i)) {
                const Particle* p = particlePool.GetParticle(i);
                ParticleData pData;
                pData.x = p->pos.x;
                pData.y = p->pos.y;
                pData.vx = p->vel.x;
                pData.vy = p->vel.y;
                pData.life = p->life;
                pData.color = 0xFFFFFFFF;
                Serializer::SerializeParticle(pData, ptr);
            }
        }

        std::string data = "FULLSTATE";
        data.append(buffer.data(), buffer.size());
        network.sendPacket(data);
    }
}