#pragma once

#include <vector>
#include "IntVector.h"
#include "raylib.h"

#define PARTICLE_TYPE_COUNT 6

#define WATER_BLUE       CLITERAL(Color){ 0, 121, 241, 150 } 
#define WATER_BLUE_2     CLITERAL(Color){ 0, 101, 221, 130 } 

const float gravity = 0.2f;

enum ParticleType {
    EMPTY = 0,
    STONE,
    DOWN_ONLY,
    SAND,
    WATER,
    STEAM
};

struct Particle {
    ParticleType type;
    Vector2      velocity = { 0, 0 };
    Color        colour = CLITERAL(Color) { 0, 0, 0, 100 };
};

struct ParticleParams {
    const std::vector<Vector2>* movement_type;
    const float       density;
    const int         drag;
    const float       grav;
    const float       max_vel;
    const std::string type_name;
};

// Movement Directions
static const std::vector<Vector2> MT_STATIC      = { {0, 0} };
static const std::vector<Vector2> MT_DOWN_ONLY   = { {0, 1} };
static const std::vector<Vector2> MT_POWDER      = { {0, 1}, {-1, 1}, {1, 1} };
static const std::vector<Vector2> MT_LIQUID      = { {0, 1}, {-1, 1}, {1, 1}, {-1, 0}, {1, 0} };
static const std::vector<Vector2> MT_GAS         = { {0, -1}, {-1, -1}, {1, -1}, {-1, 0}, {1, 0} };

static const ParticleParams param_ref[PARTICLE_TYPE_COUNT] = {
    {&MT_STATIC,    -1,    1,     0,    0,  "empty"},       // EMPTY
    {&MT_STATIC,     2,    1,     0,    0,  "stone"},       // STONE
    {&MT_DOWN_ONLY,  2,    1,     1,    1,  "down_only"},   // DOWN
    {&MT_POWDER,     2,    1,     1,    5,  "sand"},        // SAND
    {&MT_LIQUID,     1,    1,     1,    5,  "water"},       // WATER
    {&MT_GAS,        0,    1, -0.5f,    1,  "steam"}        // STEAM
};



