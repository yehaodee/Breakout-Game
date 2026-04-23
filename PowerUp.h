#ifndef POWERUP_H
#define POWERUP_H

#include "raylib.h"
#include "PowerUpEffect.h"
#include <memory>

// 前向声明
class Game;

class PowerUp {
public:
    Vector2 position;
    PowerUpType type;
    bool active;
    float duration;
    float speed;
    std::unique_ptr<PowerUpEffect> effect;

    PowerUp(float x, float y, PowerUpType t, Game& game);
    PowerUp(const PowerUp& other);
    PowerUp& operator=(const PowerUp&) = delete;
    void Apply(Game& game);
    void Update(float dt);
    void Draw();
};

#endif