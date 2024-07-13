#include <iostream>
#include <vector>
#include "raylib.h"
#include "raymath.h"

#include "IntVector.cpp"

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


    Particle* get_particle_at(int x, int y) {
        if (!in_bounds(x, y)) {
            return nullptr;
        }

        return &grid[index(x, y)];

    }

    int signum(float x) {
        return (x > 0) - (x < 0);
    }

    void move_towards(int x, int y, Vector2 vel) {

        int initial_x = x, initial_y = y;


        int dx = abs(vel.x);
        int dy = abs(vel.y);

        int sx = signum(vel.x);
        int sy = signum(vel.y);

        int err = dx - dy;



        while (true) {


            if ((x == initial_x +vel.x && y == initial_y +vel.y)) {
                break;
            }



            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x += sx;
                if (!check_empty_and_in_bounds(x, y)) {
                    x -= sx;
                    break;
                }
            }
            else if (e2 < dx) {
                err += dx;
                y += sy;
                if (!check_empty_and_in_bounds(x, y)) {
                    y -= sy;
                    break;
                }
            }

        }


        if (!(initial_x == x && initial_y == y)) {
            queue_update_swap_particles(IntVector(initial_x, initial_y), IntVector(x, y));
        }
    }

    bool check_empty_and_in_bounds(int x, int y) {
        return in_bounds(x, y) && get_particle_at(x, y)->type == EMPTY;
    }


    void queue_update_swap_particles(IntVector v1, IntVector v2) {
        Particle p1, p2;

        p1 = *get_particle_at(v1.x, v1.y);
        p2 = *get_particle_at(v2.x, v2.y);

        updates.push_back({ v1.x, v1.y, p2 });
        updates.push_back({ v2.x, v2.y, p1 }); 
    }

    void update_down(int x, int y) {
        Vector2 vel = {0, 1}; // temporary, should be in particle's info
        if (in_bounds(x, y + (int)vel.y)) {
            move_towards(x, y, vel);
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