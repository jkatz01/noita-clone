#pragma once
#pragma warning( disable : 4244 )

#include <iostream>
#include <vector>
#include "raylib.h"
#include "raymath.h"

#include "IntVector.h"

enum ParticleType {
	EMPTY,
    STONE,
    SAND
};

enum MovementType {
    MT_STATIC,
    MT_DOWN,
    MT_POWDER,
    MT_LIQUID,
    MT_GAS
};

struct Particle {
    ParticleType type;
};

struct ParticleUpdate {
    IntVector pos;
    Particle particle;
};


class SandTile {
public:
    Vector2     position = {0,0};
    const int   size = 100;

    Particle* grid = nullptr;
    std::vector<ParticleUpdate> updates; // Maybe instead of updates, let each Particle have a currrent state and next state

    SandTile() {
        grid = new Particle[size * size];
        for (int i = 0; i < size * size; i++) {
            grid[i].type = EMPTY;
        }
    }
    ~SandTile() {
        delete grid;
    }

    int index(IntVector pos) {
        return (size * pos.y + pos.x);
    }
    int index(int x, int y) {
        return (size * y + x);
    }

    bool InBounds(IntVector pos) {
        return ((pos.x >= 0 && pos.x < size) && (pos.y >= 0 && pos.y < size));
    }

    void AddMaterialSquare(IntVector pos, int size, ParticleType m_type) {
        for (int i = pos.x; i < pos.x + size; i++) {
            for (int j = pos.y; j < pos.y + size; j++) {
                if (InBounds(IntVector {i, j})) GetParticleAt(IntVector {i, j})->type = m_type;
            }
        }
    }

    MovementType GetMovementType(ParticleType t) {
        switch (t) {
            case EMPTY: return MT_STATIC;
            case STONE: return MT_STATIC;
            case SAND: return MT_DOWN;
            default: return MT_STATIC;
        }
    }


    Particle* GetParticleAt(IntVector pos) {
        if (!InBounds(pos)) {
            return nullptr;
        }

        return &grid[index(pos)];

    }

    int signum(float x) {
        return (x > 0) - (x < 0);
    }

    void MoveTowards(IntVector pos, Vector2 vel) {

        int initial_x = pos.x, initial_y = pos.y;

        int dx = abs(vel.x);
        int dy = abs(vel.y);

        int sx = signum(vel.x);
        int sy = signum(vel.y);

        int err = dx - dy;

        while (true) {
            if ((pos.x == initial_x +vel.x && pos.y == initial_y +vel.y)) {
                break;
            }
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                pos.x += sx;
                if (!CheckEmptyAndInBounds(IntVector { pos.x, pos.y})) {
                    pos.x -= sx;
                    break;
                }
            }
            else if (e2 < dx) {
                err += dx;
                pos.y += sy;
                if (!CheckEmptyAndInBounds(IntVector { pos.x, pos.y})) {
                    pos.y -= sy;
                    break;
                }
            }

        }

        if (!(initial_x == pos.x && initial_y == pos.y)) {
            QueueUpdateSwapParticles(IntVector(initial_x, initial_y), IntVector(pos.x, pos.y));
        }
    }

    bool CheckEmptyAndInBounds(IntVector pos) {
        return InBounds(pos) && GetParticleAt(pos)->type == EMPTY;
    }


    void QueueUpdateSwapParticles(IntVector v1, IntVector v2) {
        Particle p1, p2;

        p1 = *GetParticleAt(v1);
        p2 = *GetParticleAt(v2);

        updates.push_back({ IntVector {v1.x, v1.y}, p2 });
        updates.push_back({ IntVector {v2.x, v2.y}, p1 });
    }

    void UpdateDown(IntVector pos) {
        Vector2 vel = {0, 1}; // temporary, should be in particle's info
        if (InBounds(IntVector {pos.x, pos.y + (int)vel.y} )) {
            MoveTowards(pos, vel);
        }
        else {
            // Send update to chunk below
        }
    }

    void UpdateParticle(IntVector pos) {
        switch (GetMovementType(GetParticleAt(pos)->type)) {
            case MT_STATIC: return;
            case MT_DOWN: {
                UpdateDown(pos);
                return;
            }
        }
    }

    void IterateTile() {
        // Update every particle based on old grid
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                UpdateParticle(IntVector {i, j});
            }
        }

        // Update grid
        for (ParticleUpdate pu : updates) {
            grid[index(pu.pos)] = pu.particle;
        }
        updates.clear();
    }
};