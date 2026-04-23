#include "PowerUp.h"
#include "Game.h"
#include "PowerUpEffect.h"
#include <cmath>

PowerUp::PowerUp(float x, float y, PowerUpType t, Game& game) {
    position.x = x;
    position.y = y;
    type = t;
    active = true;
    duration = 10.0f;
    speed = 200.0f;
    effect = CreatePowerUpEffect(type, game);
}

PowerUp::PowerUp(const PowerUp& other) {
    position = other.position;
    type = other.type;
    active = other.active;
    duration = other.duration;
    speed = other.speed;
    effect = nullptr;
}

void PowerUp::Apply(Game& game) {
    if (effect) {
        effect->Apply(game);
    }
}

void PowerUp::Update(float dt) {
    if (active) {
        position.y += speed * dt;

        if (duration > 0) {
            duration -= dt;
            if (duration <= 0) {
                active = false;
            }
        }
    }
}

void PowerUp::Draw() {
    if (active) {
        switch (type) {
            case PowerUpType::PADDLE_EXTEND:
                DrawCircleV(position, 10, GREEN);
                break;
            case PowerUpType::MULTI_BALL:
                DrawCircleV(position, 10, BLUE);
                break;
            case PowerUpType::SLOW_BALL:
                DrawCircleV(position, 10, YELLOW);
                break;
        }
    }
}