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

    SandTile(int _size, IntVector _position) {
        size = _size;
        position = _position;
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

    float rand_range(float min, float max)
    {
        float random = (float)rand() / RAND_MAX;
        float range = max - min;
        return (random * range) + min;
    }

    Color ColorLookup(ParticleType type) {
        switch (type) {
        case EMPTY: return  CLITERAL(Color) { 0, 0, 0, 100 };
        case STONE: return  ColorBrightness(DARKBROWN, rand_range(-0.3f, 0.1f));;
        case SAND:  return  ColorBrightness(BROWN, rand_range(-0.3f, 0.3f));
        case WATER: return  WATER_BLUE;
        case STEAM: return  ColorTint(WHITE, ColorBrightness(BLUE, rand_range(0.5, 0.7f)));
        default:    return  RED;
        }
    }

    void AddMaterialSquare(IntVector pos, int size, ParticleType m_type) {
        for (int i = pos.x; i < pos.x + size; i++) {
            for (int j = pos.y; j < pos.y + size; j++) {
                if (InBounds(IntVector {i, j})) {
                    Particle *p = GetParticleAt(IntVector{ i, j });
                    if (p->type == EMPTY) {
                        p->type = m_type;
                        p->colour = ColorLookup(m_type);
                    }
                }

            }
        }
    }

    void AddMaterialCircle(IntVector pos, int size, ParticleType m_type) {
        size = size/2;
        for (int i = - size; i < size; i++) {
            for (int j = - size; j < size; j++) {
                if (i * i + j * j <= size * size) {
                    if (InBounds({ i + pos.x, j + pos.y })) {
                        Particle *p = GetParticleAt({ i + pos.x, j + pos.y });
                        if (p->type == EMPTY) {
                            p->type = m_type;
                            p->colour = ColorLookup(m_type);
                        }
                    }
                }
            }
        }
    }

    void DeleteMaterialCircle(IntVector pos, int size) {
        size = size / 2;
        for (int i = -size; i < size; i++) {
            for (int j = -size; j < size; j++) {
                if (i * i + j * j <= size * size) {
                    if (InBounds({ i + pos.x, j + pos.y })) {
                        Particle* p = GetParticleAt({ i + pos.x, j + pos.y });
                        p->type = EMPTY;
                        p->colour = ColorLookup(EMPTY);
                    }
                }
            }
        }
    }

    // Maybe better to copy value?
    const std::vector<Vector2>* GetMovementDirections(ParticleType t) {
        return param_ref[t].movement_type;
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

        if ( param_ref[p1->type].density > param_ref[p2->type].density) {
            p1->velocity = {0,0}; // SIDE EFFECT: Changes velocity
            return true;
        }
        return false;
    }

    void ApplyGravity(Particle* p) {
        if (abs(p->velocity.y) > param_ref[p->type].max_vel) {
            return;
        }
        p->velocity.y += gravity * param_ref[p->type].grav;
    }
    void ApplyDrag(Particle* p) {
        if (abs(p->velocity.y) < 1) {
            return;
        }
        int sx = signum((int)p->velocity.x);
        p->velocity.x -= sx * param_ref[p->type].drag;
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

        // for chunks:
        // let particles go out of bounds
        // if destination is beyond bounds: pass it to appropriate chunk
        // translate position
        // check if first pixel is movable to
        // restart MoveTowards in new chunk and do it on same frame
        
        IntVector end_pos = MoveVelocity(pos, p->velocity);
        if (!(end_pos.x == pos.x && end_pos.y == pos.y)) {
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