#ifndef POWERUPEFFECT_H
#define POWERUPEFFECT_H

#include <memory>
#include "Paddle.h"

class Game;

enum class PowerUpType {
    PADDLE_EXTEND,
    MULTI_BALL,
    SLOW_BALL
};

class PowerUpEffect {
public:
    virtual void Apply(Game& game, Paddle& paddle) = 0;
    virtual ~PowerUpEffect() {}
};

class ExtendPaddleEffect : public PowerUpEffect {
    float extraWidth;
    float duration;
public:
    ExtendPaddleEffect(float w, float d) : extraWidth(w), duration(d) {}
    void Apply(Game& game, Paddle& paddle) override;
};

class MultiBallEffect : public PowerUpEffect {
public:
    void Apply(Game& game, Paddle& paddle) override;
};

class SlowBallEffect : public PowerUpEffect {
    float duration;
public:
    SlowBallEffect(float d) : duration(d) {}
    void Apply(Game& game, Paddle& paddle) override;
};

std::unique_ptr<PowerUpEffect> CreatePowerUpEffect(PowerUpType type, Game& game);

#endif