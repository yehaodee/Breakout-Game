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

        for (int i = 0; i < game.powerUpConfig[1].extraBalls; i++) {
            float angle = (float)rand() / RAND_MAX * PI * 2;
            float speed = game.ballSpeed;
            Vector2 velocity = { cos(angle) * speed, sin(angle) * speed };

            Vector2 position = { originalPos.x + (float)(rand() % 20 - 10), originalPos.y + (float)(rand() % 20 - 10) };

            game.balls.emplace_back(position, velocity, game.ballRadius, RED);
        }
    }
}

void SlowBallEffect::Apply(Game& game) {
    game.slowBallEffectTime = duration;
    for (auto& ball : game.balls) {
        Vector2 vel = ball.GetVelocity();
        ball.SetVelocity({ vel.x * game.powerUpConfig[2].speedFactor, vel.y * game.powerUpConfig[2].speedFactor });
    }
}

std::unique_ptr<PowerUpEffect> CreatePowerUpEffect(PowerUpType type, Game& game) {
    switch (type) {
        case PowerUpType::PADDLE_EXTEND:
            return std::make_unique<ExtendPaddleEffect>(game.powerUpConfig[0].extraWidth, game.powerUpConfig[0].duration);
        case PowerUpType::MULTI_BALL:
            return std::make_unique<MultiBallEffect>();
        case PowerUpType::SLOW_BALL:
            return std::make_unique<SlowBallEffect>(game.powerUpConfig[2].duration);
        default:
            return nullptr;
    }
}