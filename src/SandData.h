#pragma once

#include <vector>
#include "IntVector.h"

#define PARTICLE_TYPE_COUNT 4

const float gravity = 1;

enum ParticleType {
    EMPTY,
    STONE,
    DOWN_ONLY,
    SAND,
    
};

struct Particle {
    ParticleType type;
    Vector2      velocity = { 0, 0 };
    Color        colour = YELLOW;
};

const std::vector<IntVector> MT_STATIC = { {0, 0} };
const std::vector<IntVector> MT_DOWN_ONLY = { {0, 1} };
const std::vector<IntVector> MT_POWDER = { {0, 1}, {-1, 1}, {1, 1} };