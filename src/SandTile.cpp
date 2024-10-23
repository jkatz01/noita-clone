#pragma once
#pragma warning( disable : 4244 )

#include <iostream>
#include <vector>
#include "raylib.h"
#include "raymath.h"

#include "IntVector.h"
#include "SandData.h"
#include "NeighbourTD.h"

struct ParticleUpdate {
    IntVector source;
    IntVector dest;
};

// Applies all moves from neighbours from previous frames
struct ParticleUpdateN_Move { 
    Particle  p;
    IntVector dest;
};

class SandTile {
public:
    IntVector     position = {0,0};
    int           tile_size = 100;

    Particle* grid = nullptr;
    std::vector<ParticleUpdate>         updates; 
    std::vector<ParticleUpdateN_Move>   updates_to_neighours;

    SandTile* tile_neighbours[8] = {nullptr}; // Starts at top middle, goes clockwise

    SandTile(int _tile_size, IntVector _position) {
        tile_size = _tile_size;
        position = _position;
        if (tile_size < 10) tile_size = 10;
        int g_tile_size = tile_size * tile_size;

        grid = new Particle[g_tile_size];
        for (int i = 0; i < g_tile_size; i++) {
            grid[i].type = EMPTY;
        }
    }
    ~SandTile() {
        delete grid;
    }

    void Addtile_n(NeighbourTD index, SandTile* t) {
        if (index < 0 || index > 7) {
            return;
        }
        tile_neighbours[index] = t;
    }

    int index(IntVector pos) {
        return (tile_size * pos.y + pos.x);
    }

    int index(int x, int y) {
        return (tile_size * y + x);
    }

    bool InBounds(IntVector pos) {
        return ((pos.x >= 0 && pos.x < tile_size) && (pos.y >= 0 && pos.y < tile_size));
    }

    bool NeighbourExists(NeighbourTD index) {
        if (index != ND_MYSELF && tile_neighbours[index] != NULL) {
            return true;
        }
        return false;
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


    // Returns {-1, -1} if false
    IntVector ReplacePositionWithNeighbour(IntVector pos, Particle *p, NeighbourTD* neighbour_moved_to) {
        NeighbourTD n = NeighbourFromPosition(pos, tile_size);

        *neighbour_moved_to = ND_MYSELF; // Fail by default

        if (!NeighbourExists(n)) {
            return { -1, -1 };
        }
        if (n != ND_MYSELF) {
            // if we can replace the particle at the neighbour
            IntVector pos_in_neighbour = TranslateParticleToNeighbour(pos, tile_size);
            

            if (tile_neighbours[n]->CanReplaceParticleN(p, pos_in_neighbour)) {
                *neighbour_moved_to = n;
                return pos_in_neighbour;
            }
        }
        return {-1, -1};
    }

    // Returns translated position if moved to a neighbour
    IntVector MoveVelocity(IntVector pos, Vector2 vel, NeighbourTD *neighbour_moved_to) {

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

                IntVector neighbour_pos = ReplacePositionWithNeighbour(pos, initial_p, neighbour_moved_to);
                if (*neighbour_moved_to != ND_MYSELF) {
                    return neighbour_pos;
                }

                if (!CanReplaceParticle(ipos, pos)) {
                    pos.x -= sx;
                    initial_p->velocity.x = 0; // SIDE EFFECT: Changes velocity
                    break;
                }
            }
            else if (e2 < dx) {
                err += dx;
                pos.y += sy;

                IntVector neighbour_pos = ReplacePositionWithNeighbour(pos, initial_p, neighbour_moved_to);
                if (*neighbour_moved_to != ND_MYSELF) {
                    return neighbour_pos;
                }

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

    bool CanReplaceParticle(IntVector v_src, IntVector v_dst) {
        if (!InBounds(v_dst)) {
            return false;
        }
        if (CheckEmptyAndInBounds(v_dst)) {
            return true;
        }
        
        Particle* p1 = GetParticleAt(v_src);
        Particle* p2 = GetParticleAt(v_dst);

        if ( param_ref[p1->type].density > param_ref[p2->type].density) {
            p1->velocity = {0,0}; // SIDE EFFECT: Changes velocity
            return true;
        }
        return false;
    }
    bool CanReplaceParticleN(Particle *p_src, IntVector v_dst) {
        if (!InBounds(v_dst)) {
            return false;
        }
        if (CheckEmptyAndInBounds(v_dst)) {
            return true;
        }
        
        Particle* p2 = GetParticleAt(v_dst);

        if ( param_ref[p_src->type].density > param_ref[p2->type].density) {
            p_src->velocity = {0,0}; // SIDE EFFECT: Changes velocity
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

    bool AbsVelocityLessThan(int value, Particle *p) {
        return (abs(p->velocity.x) < value && abs(p->velocity.y) < value);
    }

    void UpdateParticle(IntVector pos) {
        Particle* p = GetParticleAt(pos);

        if (p->type == EMPTY) {
            return;
        }
        
        if (!AbsVelocityLessThan(1, p)) {
            ApplyDrag(p);
        }
        else { // get new velocity
            const std::vector<Vector2>* mv = GetMovementDirections(p->type);
            for (Vector2 dir : *mv) {
                IntVector new_pos = { pos.x + (int)dir.x, pos.y + (int)dir.y };

                if (InBounds(new_pos) && CanReplaceParticle(pos, new_pos)) {
                    p->velocity = { dir.x, dir.y };
                    break;
                }
                else {
                    NeighbourTD n_index = NeighbourFromPosition(new_pos, tile_size);
                    if (n_index != ND_MYSELF && NeighbourExists(n_index)) {
                        IntVector pos_in_neighbour = TranslateParticleToNeighbour(new_pos, tile_size);
                        bool can_replace = tile_neighbours[n_index]->CanReplaceParticleN(p, pos_in_neighbour);
                        if (can_replace) {
                            p->velocity = { dir.x, dir.y };
                            break;
                        }
                    }
                }
            }
        }

        ApplyGravity(p);

        NeighbourTD n_moved_to = ND_MYSELF;
        IntVector end_pos = MoveVelocity(pos, p->velocity, &n_moved_to);

        if (n_moved_to == ND_MYSELF) {
            if (!(end_pos == pos)) {
                DrawStupidLines(pos, end_pos);
                QueueUpdateSwapParticles(pos, end_pos);
            }
        }
        else {
            // add to vector of NeighbourUpdates that applies all moves beforehand in neighbour 
            // replace current pos with the particle from end_pos
            QueueNeighbourMovementParticle(end_pos, *p, n_moved_to);
            SwapParticle(*tile_neighbours[n_moved_to]->GetParticleAt(end_pos), pos);
        }
        
    }

    void DrawStupidLines(IntVector pos, IntVector end_pos) {
        int duh = 800 / 100 / 4;
        IntVector guh = { (position.x * tile_size * duh), (position.y * tile_size * duh) };

        DrawLine(guh.x + pos.x * duh + 1, guh.y + pos.y * duh + 1, guh.x + end_pos.x * duh + 1, guh.y + end_pos.y * duh + 1, WHITE);
    }

    void QueueUpdateSwapParticles(IntVector v_src, IntVector v_dst) {
        updates.push_back({ v_src, v_dst });
    }

    void QueueNeighbourMovementParticle(IntVector v_src, Particle p, NeighbourTD n_index) {
        if (!NeighbourExists(n_index)) {
            return;
        }
        tile_neighbours[n_index]->updates_to_neighours.push_back({p, v_src});
    }

    // swaps source with destination
    void SwapParticles(IntVector src, IntVector dst) {
        Particle temp = *GetParticleAt(src);
        grid[index(src)] = *GetParticleAt(dst);
        grid[index(dst)] = temp;
    }

    // swaps destination for any particle
    void SwapParticle(Particle new_p, IntVector dst) {
        grid[index(dst)] = new_p;
    }


    void IterateTileAlternate() {
        // Apply moves from neighbours from previous frames
        for (ParticleUpdateN_Move& pnu : updates_to_neighours) {
            SwapParticle(pnu.p, pnu.dest);
        }
        updates_to_neighours.clear();

        // Update every particle based on old grid
        for (int i = 0; i < tile_size; i++) {
            if (i % 2 == 0) {
                for (int j = 0; j < tile_size; j++) {
                    UpdateParticle(IntVector{ j, i });
                }
            }
            else {
                for (int j = tile_size - 1; j >= 0; j--) {
                    UpdateParticle(IntVector{ j, i });
                }
            }
        }
        // Update grid
        // TODO: -----------------------------------
        // remove moves where destination has been filled
        for (ParticleUpdate& pu : updates) {
            SwapParticles(pu.source, pu.dest);
        }
        updates.clear();
    }
};