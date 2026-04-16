#include "PowerUp.h"
#include "Game.h"
#include "PowerUpEffect.h"
#include <cmath>

PowerUp::PowerUp(float x, float y, PowerUpType t, Game& game) {
    position.x = x;
    position.y = y;
    type = t;
    active = true;
    duration = 10.0f; // 默认持续10秒
    speed = 200.0f;   // 默认掉落速度
    effect = CreatePowerUpEffect(type, game);
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