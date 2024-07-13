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

    void move_towards2(int x, int y, Vector2 vel) {

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

    void move_towards(int x, int y, Vector2 vel) {


        Particle particle = *get_particle_at(x, y);
        if (particle.type == EMPTY) {
            //THERE IS NO PARTICLE - bald kid with a spoon
            return;
        }

        float offset_x = 0.5, offset_y = 0.5;

        Vector2 norm_vel = Vector2Normalize(vel);

        IntVector* cornerDirection = new IntVector(signum(vel.x), signum(vel.y));


        int cur_x, cur_y, dest_x, dest_y;

        cur_x = x;
        cur_y = y;
        dest_x = x + vel.x;
        dest_y = y + vel.y;

        Vector2 candidate_step_pos = { cur_x, cur_y };

        float step_scale = 1.41421356237f;
        bool blocked = false;
        //bool stepped;

        while ((cur_x != dest_x || cur_y != dest_y) && !blocked) {

            //perfect accuracy get true step_scale for 1 cell
            float dx = cornerDirection->x + cur_x - (candidate_step_pos.x);
            float dy = cornerDirection->y + cur_x - (candidate_step_pos.y);
            step_scale = sqrt(dx * dx + dy * dy) + 0.001;


            while (true) {


                Vector2 scaledVector = Vector2Scale(norm_vel, step_scale);

                //move closer
                candidate_step_pos = Vector2Add(candidate_step_pos, scaledVector);

                //get cell
                int cell_x, cell_y;
                cell_x = round(candidate_step_pos.x);
                cell_y = round(candidate_step_pos.y);

                //check manhattan distance
                int man_dist = abs(cell_x - cur_x) + abs(cell_y - cur_y);

                if (man_dist == 1) {

                    if (get_particle_at(cell_x, cell_y)->type == EMPTY) {
                        //success
                        cur_x = cell_x;
                        cur_y = cell_y;
                    }
                    else {
                        blocked = true;
                    }
                    break;
                }

                if (man_dist == 0) {
                    /*
                    //move more
                    if (step_scale == 1) {
                        step_scale = 0.25f; //to speed it up
                    }
                    else {
                        step_scale /= 2;
                    }
                    */
                    continue;
                }

                if (man_dist >=2) {
                    step_scale = -abs(step_scale/2);
                    continue;
                }
            }



        }
        if (!(cur_x == x && cur_y == y)) {
            queue_update_swap_particles(IntVector(cur_x, cur_y), IntVector(x, y));
        }


    }


    void move_by_velocity(int x, int y, Vector2 vel) {
        int cur_x = x;
        int cur_y = y;
        int desired_x = x;
        int desired_y = y + (int)vel.y;

        bool y_down = (vel.y > 0);

        while (cur_x != desired_x || cur_y != desired_y) {
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
            move_towards2(x, y, vel);
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