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

// Movement Directions
const std::vector<Vector2> MT_STATIC      = { {0, 0} };
const std::vector<Vector2> MT_DOWN_ONLY   = { {0, 1} };
const std::vector<Vector2> MT_POWDER      = { {0, 1}, {-1, 1}, {1, 1} };
const std::vector<Vector2> MT_LIQUID      = { {0, 1}, {-1, 1}, {1, 1}, {-1, 0}, {1, 0} };
const std::vector<Vector2> MT_GAS         = { {0, -1}, {-1, -1}, {1, -1}, {-1, 0}, {1, 0} };

// Particle Type Movement Direction Reference

static const std::vector<Vector2>* direction_ref[PARTICLE_TYPE_COUNT] = {
    &MT_STATIC, //EMPTY
    &MT_STATIC,
    &MT_DOWN_ONLY,
    &MT_POWDER,
    &MT_LIQUID, //WATER
    &MT_GAS
};

// TODO: Make these shorts stored in the particle?
static const float density_ref[PARTICLE_TYPE_COUNT] = {
    -1, //EMPTY
    2,
    2,
    2,
    1, //WATER
    0
};

static const int drag_ref[PARTICLE_TYPE_COUNT] = {
    1, //EMPTY
    1,
    1,
    1, //SAND
    0,  //WATER
    1
};

static const float grav_ref[PARTICLE_TYPE_COUNT] = {
    0, //EMPTY
    0, //STONE
    1, //DOWN ONLY
    1, //SAND
    1,  //WATER
    -0.5  //STEAM
};

static const float max_vel_ref[PARTICLE_TYPE_COUNT] = {
    0, //EMPTY
    0, //STONE
    1, //DOWN ONLY
    5, //SAND
    5,  //WATER
    1  //STEAM
};

//

static const std::string type_names[PARTICLE_TYPE_COUNT] = {
    "empty", "stone", "down_only", "sand", "water"
};