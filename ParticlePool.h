#ifndef PARTICLEPOOL_H
#define PARTICLEPOOL_H

#include "Particle.h"
#include <array>

const int MAX_PARTICLES = 500;

class ParticlePool {
private:
    std::array<Particle, MAX_PARTICLES> particles;
    bool active[MAX_PARTICLES];
    int freeIndex;
    
public:
    ParticlePool();
    ~ParticlePool();
    
    Particle* Create(const Vector2& pos, const Vector2& vel, const Color& color, float life);
    void Update(float dt);
    void Draw();
    void Clear();
    
    int GetActiveCount() const;
    const Particle* GetParticle(int index) const;
    bool IsActive(int index) const;
    int GetMaxParticles() const { return MAX_PARTICLES; }
};

#endif
