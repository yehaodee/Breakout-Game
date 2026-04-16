#include "PowerUpEffect.h"
#include "Game.h"
#include <cmath>

void ExtendPaddleEffect::Apply(Game& game) {
    game.paddle.Extend(extraWidth, duration);
}

void MultiBallEffect::Apply(Game& game) {
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
}

void SlowBallEffect::Apply(Game& game) {
    game.slowBallEffectTime = duration; // 持续时间
    for (auto& ball : game.balls) {
        Vector2 vel = ball.GetVelocity();
        ball.SetVelocity({ vel.x * 0.7f, vel.y * 0.7f });
    }
}

std::unique_ptr<PowerUpEffect> CreatePowerUpEffect(PowerUpType type) {
    switch (type) {
        case PowerUpType::PADDLE_EXTEND:
            return std::make_unique<ExtendPaddleEffect>(50.0f, 10.0f);
        case PowerUpType::MULTI_BALL:
            return std::make_unique<MultiBallEffect>();
        case PowerUpType::SLOW_BALL:
            return std::make_unique<SlowBallEffect>(5.0f);
        default:
            return nullptr;
    }
}