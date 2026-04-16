#include "PowerUp.h"
#include "Game.h"

PowerUp::PowerUp(float x, float y, PowerUpType t) {
    position.x = x;
    position.y = y;
    type = t;
    active = true;
    duration = 10.0f; // 默认持续10秒
    speed = 200.0f;   // 默认掉落速度
    effect = CreatePowerUpEffect(type);
}

void PowerUp::Apply(Game& game) {
    if (effect) {
        effect->Apply(game);
    }
}

void PowerUp::Update(float dt) {
    // 实现增强道具的更新逻辑
    if (active) {
        // 掉落运动
        position.y += speed * dt;
        
        // 检查持续时间
        if (duration > 0) {
            duration -= dt;
            if (duration <= 0) {
                active = false;
            }
        }
    }
}

void PowerUp::Draw() {
    // 实现增强道具的绘制逻辑
    if (active) {
        // 根据类型绘制不同的道具
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