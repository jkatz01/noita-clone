#include <iostream>
#include <vector>
#include "raylib.h"

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
    int x;
    int y;
    Particle particle;
};


class SandChunk {
public:
    Vector2     position = {0,0};
    const int   size = 100;

    Particle* grid = nullptr;
    std::vector<ParticleUpdate> updates; // Maybe instead of updates, let each Particle have a currrent state and next state

    SandChunk() {
        grid = new Particle[size * size];
        for (int i = 0; i < size * size; i++) {
            grid[i].type = EMPTY;
        }
    }
    ~SandChunk() {
        delete grid;
    }

    int index(int x, int y) {
        return (size * y + x);
    }

    bool in_bounds(int x, int y) {
        return ((x >= 0 && x < size) && (y >= 0 && y < size));
    }

    void add_material_square(int x, int y, int size, ParticleType m_type) {
        for (int i = x; i < x + size; i++) {
            for (int j = y; j < y + size; j++) {
                if (in_bounds(i, j)) grid[(index(i, j))].type = m_type;
            }
        }
    }

    MovementType get_movement_type(ParticleType t) {
        switch (t) {
            case EMPTY: return MT_STATIC;
            case STONE: return MT_STATIC;
            case SAND: return MT_DOWN;
            default: return MT_STATIC;
        }
    }

    void move_by_velocity(int x, int y, Vector2 vel) {
        int cur_x = x;
        int cur_y = y;
        int desired_x = x;
        int desired_y = y + (int)vel.y;

        bool y_down = (vel.y > 0);

        while (cur_x != desired_x || cur_y != desired_y) {
            std::cout << "in loop " << x << ", " << y << ", " << desired_y << std::endl;
            if (y_down && grid[index(cur_x, cur_y + 1)].type == EMPTY) {
                cur_y++;
            }
            else {
                break;
            }
        }

        updates.push_back({ x, y, grid[index(cur_x, cur_y)] }); // Change starting position to empty
        updates.push_back({ cur_x, cur_y, grid[index(x, y)] }); // Change end position to current particle
    }

    void update_down(int x, int y) {
        Vector2 vel = {0, 1}; // temporary, should be in particle's info
        if (in_bounds(x, y + (int)vel.y)) {
            move_by_velocity(x, y, vel);
        }
        else {
            // Send update to chunk below
        }
    }

    void update_particle(int x, int y) {
        switch (get_movement_type(grid[index(x, y)].type)) {
            case MT_STATIC: return;
            case MT_DOWN: {
                update_down(x, y);
                return;
            }
        }
    }

    void iterate_chunk() {
        // Update every particle based on old grid
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                update_particle(i, j);
            }
        }

        // Update grid
        for (ParticleUpdate pu : updates) {
            grid[index(pu.x, pu.y)] = pu.particle;
        }
        updates.clear();
    }
};