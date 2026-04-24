#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H

#include "raylib.h"
#include <cstring>
#include <vector>

#pragma pack(push, 1)

struct BallData {
    float x, y;
    float vx, vy;
    float radius;
    unsigned int color;
};

struct PaddleData {
    float x, y;
    float width;
};

struct BrickData {
    float x, y;
    float width, height;
    bool active;
    int health;
};

struct PowerUpData {
    float x, y;
    bool active;
    int type;
};

struct ParticleData {
    float x, y;
    float vx, vy;
    float life;
    unsigned int color;
};

struct GameStateHeader {
    int score;
    int lives;
    int level;
    float ballSpeed;
    float slowBallEffectTime;
    int ballCount;
    int paddleBottomCount;
    int paddleTopCount;
    int brickCount;
    int powerUpCount;
    int particleCount;
};

#pragma pack(pop)

class Serializer {
public:
    static void SerializeBall(const BallData& ball, char*& buffer) {
        memcpy(buffer, &ball, sizeof(BallData));
        buffer += sizeof(BallData);
    }

    static BallData DeserializeBall(const char*& buffer) {
        BallData ball;
        memcpy(&ball, buffer, sizeof(BallData));
        buffer += sizeof(BallData);
        return ball;
    }

    static void SerializePaddle(const PaddleData& paddle, char*& buffer) {
        memcpy(buffer, &paddle, sizeof(PaddleData));
        buffer += sizeof(PaddleData);
    }

    static PaddleData DeserializePaddle(const char*& buffer) {
        PaddleData paddle;
        memcpy(&paddle, buffer, sizeof(PaddleData));
        buffer += sizeof(PaddleData);
        return paddle;
    }

    static void SerializeBrick(const BrickData& brick, char*& buffer) {
        memcpy(buffer, &brick, sizeof(BrickData));
        buffer += sizeof(BrickData);
    }

    static BrickData DeserializeBrick(const char*& buffer) {
        BrickData brick;
        memcpy(&brick, buffer, sizeof(BrickData));
        buffer += sizeof(BrickData);
        return brick;
    }

    static void SerializePowerUp(const PowerUpData& powerUp, char*& buffer) {
        memcpy(buffer, &powerUp, sizeof(PowerUpData));
        buffer += sizeof(PowerUpData);
    }

    static PowerUpData DeserializePowerUp(const char*& buffer) {
        PowerUpData powerUp;
        memcpy(&powerUp, buffer, sizeof(PowerUpData));
        buffer += sizeof(PowerUpData);
        return powerUp;
    }

    static void SerializeParticle(const ParticleData& particle, char*& buffer) {
        memcpy(buffer, &particle, sizeof(ParticleData));
        buffer += sizeof(ParticleData);
    }

    static ParticleData DeserializeParticle(const char*& buffer) {
        ParticleData particle;
        memcpy(&particle, buffer, sizeof(ParticleData));
        buffer += sizeof(ParticleData);
        return particle;
    }

    static void SerializeHeader(const GameStateHeader& header, char*& buffer) {
        memcpy(buffer, &header, sizeof(GameStateHeader));
        buffer += sizeof(GameStateHeader);
    }

    static GameStateHeader DeserializeHeader(const char*& buffer) {
        GameStateHeader header;
        memcpy(&header, buffer, sizeof(GameStateHeader));
        buffer += sizeof(GameStateHeader);
        return header;
    }

    static size_t CalculateBufferSize(int ballCount, int brickCount, int powerUpCount, int particleCount) {
        return sizeof(GameStateHeader)
             + ballCount * sizeof(BallData)
             + 2 * sizeof(PaddleData)
             + brickCount * sizeof(BrickData)
             + powerUpCount * sizeof(PowerUpData)
             + particleCount * sizeof(ParticleData);
    }
};

#endif