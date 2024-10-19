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
    int           size = 100;

    Particle* grid = nullptr;
    std::vector<ParticleUpdate> updates; 

    SandTile(int _size) {
        size = _size;
        if (size < 10) size = 10;
        int g_size = size * size;

        grid = new Particle[g_size];
        for (int i = 0; i < g_size; i++) {
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
        case STEAM: return  CLITERAL(Color) { 200, 200, 200, 255 };
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
    const std::vector<Vector2>* GetMovementDirections(ParticleType t) {
        return direction_ref[t];
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

        IntVector ivel = {(int)vel.x, (int)vel.y};
        IntVector ipos = {pos.x, pos.y};
        Particle *initial_p = GetParticleAt(pos);

        int dx = abs(ivel.x);
        int dy = abs(ivel.y);

        int sx = signum(ivel.x);
        int sy = signum(ivel.y);

        int err = dx - dy;

        while (true) {
            if ((pos.x == ipos.x + ivel.x && pos.y == ipos.y + ivel.y)) {
                break;
            }
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                pos.x += sx;
                if (!CanReplaceParticle(ipos, pos)) {
                    pos.x -= sx;
                    initial_p->velocity.x = 0; // SIDE EFFECT: Changes velocity
                    break;
                }
            }
            else if (e2 < dx) {
                err += dx;
                pos.y += sy;
                if (!CanReplaceParticle(ipos, pos)) {
                    pos.y -= sy;
                    initial_p->velocity.y = 0; // SIDE EFFECT: Changes velocity
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
        if (!InBounds(v2)) {
            return false;
        }
        if (CheckEmptyAndInBounds(v2)) {
            return true;
        }
        
        Particle* p1 = GetParticleAt(v1);
        Particle* p2 = GetParticleAt(v2);

        if ( density_ref[p1->type] > density_ref[p2->type]) {
            p1->velocity = {0,0}; // SIDE EFFECT: Changes velocity
            return true;
        }
        return false;
    }

    void ApplyGravity(Particle* p) {
        p->velocity.y += gravity * grav_ref[p->type];
    }
    void ApplyDrag(Particle* p) {
        if (abs(p->velocity.y) < 1) {
            return;
        }
        int sx = signum((int)p->velocity.x);
        p->velocity.x -= sx * drag_ref[p->type];
    }

    void UpdateParticle(IntVector pos) {
        Particle* p = GetParticleAt(pos);

        if (p->type == EMPTY) {
            return;
        }

        // get new velocity
        if (abs(p->velocity.x) < 1 && abs(p->velocity.y) < 1) {
            const std::vector<Vector2>* mv = GetMovementDirections(p->type);
            for (Vector2 dir : *mv) {
                IntVector new_pos = { pos.x + (int)dir.x, pos.y + (int)dir.y };

                if (InBounds(new_pos) && CanReplaceParticle(pos, new_pos)) {
                    p->velocity = { dir.x, dir.y };
                    break;
                }
            }
        }
        else {
            ApplyDrag(p);
        }
        
        ApplyGravity(p);
        
        IntVector end_pos = MoveVelocity(pos, p->velocity);
        if (!(end_pos.x == pos.x && end_pos.y == pos.y)) {
            DrawLine(pos.x*2 + 1, pos.y*2 + 1, end_pos.x*2 + 1, end_pos.y*2 + 1, WHITE);
            QueueUpdateSwapParticles(pos, end_pos);
        }
    }


    void IterateTileAlternate() {
        // Update every particle based on old grid
        
        for (int i = 0; i < size; i++) {
            if (i % 2 == 0) {
                for (int j = 0; j < size; j++) {
                    UpdateParticle(IntVector{ j, i });
                }
            }
            else {
                for (int j = size - 1; j >= 0; j--) {
                    UpdateParticle(IntVector{ j, i });
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