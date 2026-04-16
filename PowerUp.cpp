#include "PowerUp.h"
#include "Game.h"
#include <cmath>

PowerUp::PowerUp(float x, float y, PowerUpType t) {
    position.x = x;
    position.y = y;
    type = t;
    active = true;
    duration = 10.0f; // 默认持续10秒
    speed = 200.0f;   // 默认掉落速度
}

void PowerUp::Apply(Game& game) {
    // 实现增强道具的效果
    switch (type) {
        case PowerUpType::PADDLE_EXTEND:
            // 实现 paddle 扩展效果
            game.paddle.Extend(50.0f, duration);
            break;
        case PowerUpType::MULTI_BALL:
            // 实现多球效果
            if (!game.balls.empty()) {
                Ball& originalBall = game.balls[0];
                Vector2 originalPos = originalBall.GetPosition();
                
                // 生成两个新球，速度方向随机
                for (int i = 0; i < 2; i++) {
                    // 随机速度方向
                    float angle = (float)rand() / RAND_MAX * PI * 2;
                    float speed = game.ballSpeed;
                    Vector2 velocity = { cos(angle) * speed, sin(angle) * speed };
                    
                    // 在原始球附近生成新球
                    Vector2 position = { originalPos.x + (float)(rand() % 20 - 10), originalPos.y + (float)(rand() % 20 - 10) };
                    
                    game.balls.emplace_back(position, velocity, game.ballRadius, RED);
                }
            }
            break;
        case PowerUpType::SLOW_BALL:
            // 实现慢球效果
            game.slowBallEffectTime = 5.0f; // 持续5秒
            for (auto& ball : game.balls) {
                Vector2 vel = ball.GetVelocity();
                ball.SetVelocity({ vel.x * 0.7f, vel.y * 0.7f });
            }
            break;
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