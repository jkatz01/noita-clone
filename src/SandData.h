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



// Movement Directions
const std::vector<IntVector> MT_STATIC      = { {0, 0} };
const std::vector<IntVector> MT_DOWN_ONLY   = { {0, 1} };
const std::vector<IntVector> MT_POWDER      = { {0, 1}, {-1, 1}, {1, 1} };

// Particle Type Movement Direction Reference
struct DirectionRef {
    ParticleType type;
    const std::vector<IntVector>* directions;
};

const DirectionRef direction_ref[PARTICLE_TYPE_COUNT] = {
    {EMPTY,     &MT_STATIC},
    {STONE,     &MT_STATIC},
    {DOWN_ONLY, &MT_DOWN_ONLY},
    {SAND,      &MT_POWDER},
};