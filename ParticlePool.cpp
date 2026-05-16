#include "ParticlePool.h"
#include "raylib.h"

ParticlePool::ParticlePool() {
    Clear();
}

ParticlePool::~ParticlePool() {
}

void ParticlePool::Clear() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        active[i] = false;
        particles[i].life = 0;
    }
    freeIndex = 0;
}

Particle* ParticlePool::Create(const Vector2& pos, const Vector2& vel, const Color& color, float life) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        int idx = (freeIndex + i) % MAX_PARTICLES;
        if (!active[idx]) {
            particles[idx].pos = pos;
            particles[idx].vel = vel;
            particles[idx].color = color;
            particles[idx].life = life;
            active[idx] = true;
            freeIndex = (idx + 1) % MAX_PARTICLES;
            return &particles[idx];
        }
    }
    return nullptr;
}

void ParticlePool::Update(float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (active[i]) {
            particles[i].Update(dt);
            if (particles[i].life <= 0) {
                active[i] = false;
            }
        }
    }
}

void ParticlePool::Draw() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (active[i] && particles[i].life > 0) {
            DrawCircleV(particles[i].pos, 2, particles[i].color);
        }
    }
}

int ParticlePool::GetActiveCount() const {
    int count = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (active[i] && particles[i].life > 0) {
            count++;
        }
    }
    return count;
}

const Particle* ParticlePool::GetParticle(int index) const {
    if (index >= 0 && index < MAX_PARTICLES) {
        return &particles[index];
    }
    return nullptr;
}

bool ParticlePool::IsActive(int index) const {
    if (index >= 0 && index < MAX_PARTICLES) {
        return active[index] && particles[index].life > 0;
    }
    return false;
}
