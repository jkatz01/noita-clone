#pragma once
#pragma warning( disable : 4244 )

#include <iostream>
#include <vector>
#include "raylib.h"
#include "raymath.h"

#include "IntVector.h"
#include "SandData.h"

struct ParticleUpdate {
    IntVector source;
    IntVector dest;
};

class SandTile {
public:
    IntVector     position = {0,0};
    const int   size = 100;

    Particle* grid = nullptr;
    std::vector<ParticleUpdate> updates; 



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

    Color ColorLookup(ParticleType type) {
        switch (type) {
        case EMPTY: return CLITERAL(Color) { 0, 0, 0, 100 };
        case STONE: return DARKBROWN;
        case SAND: return YELLOW;
        case WATER: return BLUE;
        default: return RED;
        }
    }

    void AddMaterialSquare(IntVector pos, int size, ParticleType m_type) {
        for (int i = pos.x; i < pos.x + size; i++) {
            for (int j = pos.y; j < pos.y + size; j++) {
                if (InBounds(IntVector {i, j})) {
                    GetParticleAt(IntVector {i, j})->type = m_type;
                    GetParticleAt(IntVector{ i, j })->colour = ColorLookup(m_type);
                }

            }
        }
    }

    void AddMaterialCircle(IntVector pos, int size, ParticleType m_type) {
        size = size/2;
        for (int i = - size; i < size; i++) {
            for (int j = - size; j < size; j++) {
                if (i * i + j * j <= size * size) {
                    if (InBounds(IntVector{ i + pos.x, j + pos.y })) {
                        GetParticleAt(IntVector{ i + pos.x, j + pos.y })->type = m_type;
                        GetParticleAt(IntVector{ i + pos.x, j + pos.y })->colour = ColorLookup(m_type);
                    }
                }
            }
        }
    }

    // Maybe better to copy value?
    const std::vector<IntVector>* GetMovementDirections(ParticleType t) {
        for (int i = 0; i < PARTICLE_TYPE_COUNT; i++) {
            if (t == direction_ref[i].type) {
                return direction_ref[i].directions;
            }
        }
        return nullptr;
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

    IntVector MoveVelocity(IntVector pos, Vector2 vel) {

        int initial_x = pos.x, initial_y = pos.y;
        Particle *initial_p = GetParticleAt(pos);

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
                if (!CheckEmptyAndInBounds(pos)) {
                    pos.x -= sx;
                    initial_p->velocity.x = 0;
                    break;
                }
            }
            else if (e2 < dx) {
                err += dx;
                pos.y += sy;
                if (!CheckEmptyAndInBounds(pos)) {
                    pos.y -= sy;
                    initial_p->velocity.y = 0;
                    break;
                }
            }
        }

        return pos;
    }

    // can move to
    bool CheckEmptyAndInBounds(IntVector pos) {
        return InBounds(pos) && GetParticleAt(pos)->type == EMPTY;
    }


    void QueueUpdateSwapParticles(IntVector v1, IntVector v2) {

        updates.push_back({v1, v2});
    }

    bool CanReplaceParticle(IntVector v1, IntVector v2) {
        Particle *p1 = GetParticleAt(v1);
        Particle *p2 = GetParticleAt(v2);
        if (!InBounds(v2)) {
            return false;
        }
        if (p2->type == EMPTY) {
            return true;
        }
        else {
            return false;
        }
    }

    void ApplyGravity(Particle* p) {
        p->velocity.y += gravity;
    }

    void UpdateParticle(IntVector pos) {
        Particle* p = GetParticleAt(pos);
        const std::vector<IntVector> *mv = GetMovementDirections(p->type);

        for (IntVector dir : *mv) {
            IntVector new_pos = { pos.x + dir.x, pos.y + dir.y };

            if (InBounds(new_pos) && CanReplaceParticle(pos, new_pos)) {
                p->velocity = { (float)dir.x, (float)dir.y };
                break;
            }
        }
        
        IntVector end_pos = MoveVelocity(pos, p->velocity);
        p->velocity = {0, 0};
        if (!(end_pos.x == pos.x && end_pos.y == pos.y)) {
            DrawLine(pos.x*8 + 4, pos.y*8 + 4, end_pos.x*8 + 4, end_pos.y*8 + 4, WHITE);
            //std::cout << "Swapped " << pos.x << ", " << pos.y << " With " << end_pos.x << ", " << end_pos.y << std::endl;
            QueueUpdateSwapParticles(pos, end_pos);
        }
    }


    void IterateTileAlternate() {
        // Update every particle based on old grid
        
        for (int i = 0; i < size; i++) {
            if (i % 2 == 0) {
                for (int j = 0; j < size; j++) {
                    UpdateParticle(IntVector{ i, j });
                }
            }
            else {
                for (int j = size - 1; j >= 0; j--) {
                    UpdateParticle(IntVector{ i, j });
                }
            }
        }
        // Update grid
        // TODO: -----------------------------------
        // remove moves where destination has been filled
        for (ParticleUpdate& pu : updates) {
            Particle temp = *GetParticleAt(pu.source);
            grid[index(pu.source)] = *GetParticleAt(pu.dest);
            grid[index(pu.dest)] = temp;
        }
        updates.clear();
    }
};