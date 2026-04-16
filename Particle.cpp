#include "Particle.h"

void Particle::Update(float dt) {
    pos.x += vel.x * dt;
    pos.y += vel.y * dt;
    life -= dt;
}

void Particle::Draw() {
    if (life > 0) {
        DrawCircleV(pos, 2, color);
    }
}