#ifndef PARTICLE_H
#define PARTICLE_H

#include "raylib.h"

class Particle {
public:
    Vector2 pos;
    Vector2 vel;
    Color color;
    float life;
    
    void Update(float dt);
    void Draw();
};

#endif