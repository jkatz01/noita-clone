#pragma once
#pragma warning( disable : 4244 )

#include <iostream>
#include <vector>
#include <algorithm>
#include "raylib.h"
#include "raymath.h"

#include "IntVector.h"
#include "SandData.h"
#include "NeighbourTD.h"
#include "RandomRange.h"

struct ParticleUpdate {
    IntVector source;
    IntVector dest;
};

struct ParticleUpdateDraw {
    IntVector pos;
    Particle p;
};

struct TileRectangle {
    IntVector min;
    IntVector max;
};

class SandTile {
public:
    IntVector     position = {0,0};
    int           tile_size = 100;

    Particle* grid = nullptr;
    std::vector<ParticleUpdate>         updates; 
    std::vector<ParticleUpdateDraw>     update_draws;

    SandTile* tile_neighbours[8] = {nullptr}; // Starts at top middle, goes clockwise

    int simulated_cell_count = 0;
    int simulated_previous = 1;
    TileRectangle d_rec   = { {0, 0}, {0, 0} }; // Update zone
    TileRectangle d_rec_w = { {0, 0}, {0, 0} };

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

    inline void simulated_cell_add() {
        simulated_cell_count++;
    }

    inline void simulated_cell_remove() {
        simulated_cell_count--;
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

    bool InBoundsThick(IntVector pos, int thickness) {
        return ((pos.x >= 0 + thickness && pos.x < tile_size - thickness) 
                    && (pos.y >= 0 + thickness && pos.y < tile_size - thickness));
    }

    bool NeighbourExists(NeighbourTD index) {
        if (index != ND_MYSELF && tile_neighbours[index] != NULL) {
            return true;
        }
        return false;
    }

    void AddMaterialSingle(IntVector pos, ParticleType m_type) {
        if (!InBounds(pos)) return;
        Particle p = { m_type, {0, 0}, GenerateParticleColor(m_type) };
        update_draws.push_back({ pos, p });
    }

    void DeleteMaterialSingle(IntVector pos) {
        if (!InBounds(pos)) return;
        Particle* p = GetParticleAt(pos);
        if (p->type != EMPTY) {
            Particle pp = { EMPTY, {0, 0}, GenerateParticleColor(EMPTY) };
            InsertParticle(pos, pp);
        }

    }

    void AddMaterialSquare(IntVector pos, int size, ParticleType m_type) {
        for (int i = pos.x; i < pos.x + size; i++) {
            for (int j = pos.y; j < pos.y + size; j++) {
                Particle p = {m_type, {0, 0}, GenerateParticleColor(m_type) };
                update_draws.push_back({{i, j}, p});
            }
        }
    }

    void DeleteMaterialCircle(IntVector pos, int diameter) {
        if (diameter == 1) {
            diameter = 2;
        }
        diameter = diameter / 2;
        for (int i = -diameter; i < diameter; i++) {
            for (int j = -diameter; j < diameter; j++) {
                if (i * i + j * j <= diameter * diameter) {
                    if (InBounds({ i + pos.x, j + pos.y })) {
                        Particle* p = GetParticleAt({ i + pos.x, j + pos.y });
                        if (p->type != EMPTY) {
                            simulated_cell_remove();
                        }
                        p->type = EMPTY; // TAG: @cell-emptied
                        p->colour = GenerateParticleColor(EMPTY);
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

    int ClampInt(int num, int low, int high) {
        if (num < low) return low;
        if (num > high) return high;
        return num;
    }

    int ClampInTile(int num) {
        if (num < 0) return 0;
        if (num > tile_size - 1) return tile_size - 1;
        return num;
    }

    // Returns {-1, -1} if false
    NeighbourTD ReplacePositionWithNeighbourTile(IntVector pos, Particle* p) {
        NeighbourTD n = NeighbourFromPosition(pos, tile_size);


        if (!NeighbourExists(n)) {
            return ND_MYSELF; //if no neighbour the MoveVelocity function deals with it
        }
        if (n != ND_MYSELF) {
            // if we can replace the particle at the neighbour
            IntVector pos_in_neighbour = TranslateParticleToNeighbour(pos, tile_size);

            if (tile_neighbours[n]->CanReplaceParticleN(p, pos_in_neighbour)) {
                return n;
            }
            else {
                return ND_MYSELF;
                // Cant replace particle == stay in myself
            }
        }
        return ND_MYSELF;
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

                // if not in bounds
                *neighbour_moved_to = ReplacePositionWithNeighbourTile(pos, initial_p);
                if (*neighbour_moved_to != ND_MYSELF) { 
                    return pos;
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

                *neighbour_moved_to = ReplacePositionWithNeighbourTile(pos, initial_p);
                if (*neighbour_moved_to != ND_MYSELF) {
                    return pos;
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

    bool AbsVelocityLessThan(int value, Particle *p) {
        return (abs(p->velocity.x) < value && abs(p->velocity.y) < value);
    }

    bool AbsVelocityMoreEqualThan(int value, Particle* p) {
        return (abs(p->velocity.x) >= value || abs(p->velocity.y) >= value);
    }

    void MoveAndQueueParticle(IntVector pos, Particle *p) {
        NeighbourTD n_moved_to = ND_MYSELF;
        IntVector end_pos = MoveVelocity(pos, p->velocity, &n_moved_to);

        IntVector diff = VecDifference(pos, end_pos);

        if (n_moved_to == ND_MYSELF) {
            if (!(end_pos == pos)) {
                //DrawStupidLines(pos, end_pos);
                QueueUpdateSwapParticles(pos, end_pos); 
            }
            else {
                //UpdateSimZone(pos);
            }
        }
        else {
            end_pos = TranslateParticleToNeighbour(end_pos, tile_size);
            Particle pcopy = *p;
            InsertParticle(pos, *tile_neighbours[n_moved_to]->GetParticleAt(end_pos));

            // Lock mutex here?
            tile_neighbours[n_moved_to]->InsertParticle(end_pos, pcopy);
            tile_neighbours[n_moved_to]->MoveInFrameByDifference(end_pos, diff);
        }
    }

    void MoveInFrameByDifference(IntVector pos, IntVector diff) {
        Particle* p = GetParticleAt(pos);
        NeighbourTD n_moved_to = ND_MYSELF;
        IntVector end_pos = MoveVelocity(pos, {p->velocity.x - diff.x, p->velocity.y - diff.y}, &n_moved_to);

        if (n_moved_to == ND_MYSELF) {
            if (!(end_pos == pos)) {
                p->should_update = 0;
                SwapParticles(pos, end_pos); //maybe need a should_update flag?
            }
        }
        // TODO: Enable moving through more than 1 chunk per frame 
        // This seems to happen even when it shouldnt
        //else {
        //    std::cout << "moved to neighobur in frame again" << std::endl;
        //    // bug happens here
        //    end_pos = TranslateParticleToNeighbour(end_pos, tile_size);
        //    //QueueNeighbourMovementParticle(end_pos, *p, n_moved_to, diff);
        //    Particle pcopy = *p;
        //    InsertParticle(pos, *tile_neighbours[n_moved_to]->GetParticleAt(end_pos));
        //    // Lock mutex here?
        //    tile_neighbours[n_moved_to]->InsertParticle(end_pos, pcopy);
        //    tile_neighbours[n_moved_to]->MoveInFrameByDifference(end_pos, GetParticleAt(end_pos), diff);
        //}
    }

    void GetNewParticleVelocity(IntVector pos, Particle* p) {
        const std::vector<Vector2>* mv = GetMovementDirections(p->type);
        if (p->is_freefalling == 0) {
            mv = &MT_DOWN_ONLY;
        }
        for (Vector2 dir : *mv) {
            IntVector new_pos = { pos.x + (int)dir.x, pos.y + (int)dir.y };

            if (InBounds(new_pos) && CanReplaceParticle(pos, new_pos)) {
                p->velocity = { dir.x, dir.y };
                return;
            }
            else {
                NeighbourTD n_index = NeighbourFromPosition(new_pos, tile_size);
                if (n_index != ND_MYSELF && NeighbourExists(n_index)) {
                    IntVector pos_in_neighbour = TranslateParticleToNeighbour(new_pos, tile_size);
                    if (tile_neighbours[n_index]->CanReplaceParticleN(p, pos_in_neighbour)) {
                        p->velocity = { dir.x, dir.y };
                        return;
                    }
                }
            }
        }
    }

    void ApplyGravity(Particle* p) {
        if (abs(p->velocity.y) > param_ref[p->type].max_vel) {
            return;
        }
        p->velocity.y += w_gravity * param_ref[p->type].grav;
    }

    void ApplyDrag(Particle* p) {
        if (abs(p->velocity.y) < param_ref[p->type].drag_min) {
            return;
        }
        int sx = signum((int)p->velocity.x);
        p->velocity.x -= sx * w_drag;
    }

    void UpdateParticle(IntVector pos) {
        Particle* p = GetParticleAt(pos);

        if (p->type == EMPTY) {
            return;
        }
        if (p->should_update == 0) {
            p->should_update = 1;
            return;
        }

        // if freefalling move anywhere
        // if not, move down only
        
        if (p->is_freefalling == 1) {
            p->colour = BLUE;
        }
        else {
            p->colour = RED;
        }
            
        if (AbsVelocityLessThan(1, p)) { 
            GetNewParticleVelocity(pos, p);
        }
        else {
            ApplyDrag(p);
        }

        ApplyGravity(p);

        // Apply more rules
        
        p->is_freefalling = 0;
        // move function will set freefalling to 1 if something has moved ?
        MoveAndQueueParticle(pos, p);
        
    }

    void DrawStupidLines(IntVector pos, IntVector end_pos) {
        int duh = 800 / 100 / 4;
        IntVector guh = { (position.x * tile_size * duh), (position.y * tile_size * duh) };

        DrawLine(guh.x + pos.x * duh + 1, guh.y + pos.y * duh + 1, guh.x + end_pos.x * duh + 1, guh.y + end_pos.y * duh + 1, WHITE);
    }

    void QueueUpdateSwapParticles(IntVector v_src, IntVector v_dst) {
        updates.push_back({ v_src, v_dst });
    }

    // swaps source with destination
    void SwapParticles(IntVector src, IntVector dst) {
        Particle temp = *GetParticleAt(src);
        grid[index(src)] = *GetParticleAt(dst);
        grid[index(dst)] = temp;

        grid[index(src)].is_freefalling = 1;
        grid[index(dst)].is_freefalling = 1;


        UpdateSimZone(src);
        UpdateSimZone(dst);
        // need to notify neighbour if an update happened on the border
        if(!InBoundsThick(src, 1)) UpdateNeighbourZones(src);
        if(!InBoundsThick(dst, 1)) UpdateNeighbourZones(dst);
    }

    // swaps destination for any particle
    void InsertParticle(IntVector dst, Particle new_p) {
        if (new_p.type == EMPTY) {
            simulated_cell_remove();
        }
        else {
            Particle* the = GetParticleAt(dst);
            if (the->type == EMPTY) {
                simulated_cell_add();
            }
            if (the->type == new_p.type) {
                ParticleType tp = new_p.type;
                std::cout << "FUSION!!!" << std::endl; //shouldnt even get here 
                new_p.colour = GREEN;
            }
        }
        UpdateSimZone(dst);
        if (!InBoundsThick(dst, 1)) UpdateNeighbourZones(dst);
        grid[index(dst)] = new_p;
    }

    void UpdateDraws() {
        for (ParticleUpdateDraw& pud : update_draws) {
            if (InBounds(pud.pos)) {
                if (GetParticleAt(pud.pos)->type == EMPTY) {
                    InsertParticle(pud.pos, pud.p);
                }
            }
            else {
                IntVector new_pos = TranslateParticleToNeighbour(pud.pos, tile_size);
                NeighbourTD new_tile = NeighbourFromPosition(pud.pos, tile_size);
                if (NeighbourExists(new_tile)) {
                    if (tile_neighbours[new_tile]->GetParticleAt(new_pos)->type == EMPTY) {
                        tile_neighbours[new_tile]->InsertParticle(new_pos, pud.p);
                    }
                }
            }
        }
        update_draws.clear();
    }

    void UpdateNeighbourZones(IntVector src) {
        IntVector src_neighbours[8];
        Particle* p = GetParticleAt(src);
        int y_vel = std::max((int)abs(p->velocity.y), 1);
        int x_vel = std::max((int)abs(p->velocity.x), 1);

        //oh, i wasnt accounting for left/right
        if (src.x - x_vel < 0) {
            src_neighbours[ND_LEFT] = { src.x - 1, src.y };
        }
        if (src.x + x_vel >= tile_size) {
            src_neighbours[ND_RIGHT] = { src.x + 1, src.y };
        }
        if (src.y - y_vel < 0) {
            src_neighbours[ND_UP] = { src.x, src.y - 1 };
            if (src.x - x_vel < 0)
                src_neighbours[ND_UP_LEFT] = { src.x - 1, src.y - 1 };
            if (src.x + x_vel >= tile_size)
                src_neighbours[ND_UP_RIGHT] = { src.x + 1, src.y - 1 };
        }
        if (src.y + y_vel >= tile_size) {
            src_neighbours[ND_DOWN] = { src.x, src.y + 1 };
            if (src.x - x_vel < 0 ) 
                src_neighbours[ND_DOWN_LEFT] = { src.x - 1, src.y + 1 }; 
            if (src.x + x_vel >= tile_size)
                src_neighbours[ND_DOWN_RIGHT] = { src.x + 1, src.y + 1 };
        } 

        for (int i = 0; i < 8; i++) {
            if (!(src_neighbours[i] == IntVector{-9999, -9999})) {
                if (NeighbourExists((NeighbourTD)i)) {
                    tile_neighbours[i]->UpdateSimZone(TranslateParticleToNeighbour(src_neighbours[i], tile_size));
                }
            }
        }
    }

    void UpdateSimZone(IntVector point) {
        Particle *p = GetParticleAt(point);
        d_rec_w.min.x = ClampInTile(std::min(point.x - abs((int)p->velocity.x) - 2, d_rec_w.min.x));
        d_rec_w.min.y = ClampInTile(std::min(point.y - abs((int)p->velocity.y) - 2, d_rec_w.min.y));
        d_rec_w.max.x = ClampInTile(std::max(point.x + abs((int)p->velocity.x) + 2, d_rec_w.max.x));
        d_rec_w.max.y = ClampInTile(std::max(point.y + abs((int)p->velocity.y) + 2, d_rec_w.max.y));
    }

    void UpdateZoneRectangle() {
        d_rec.min.x = d_rec_w.min.x;
        d_rec.min.y = d_rec_w.min.y;
        d_rec.max.x = d_rec_w.max.x;
        d_rec.max.y = d_rec_w.max.y;

        d_rec_w.min.x = tile_size;
        d_rec_w.min.y = tile_size;
        d_rec_w.max.x = -1;
        d_rec_w.max.y = -1;
    }

    void UpdateParticles() {
        // Update every particle based on old grid
        int min_x = std::min(d_rec.min.x, d_rec_w.min.x);
        int min_y = std::min(d_rec.min.y, d_rec_w.min.y);
        int max_x = std::max(d_rec.max.x, d_rec_w.max.x);
        int max_y = std::max(d_rec.max.y, d_rec_w.max.y);

        for (int i = min_y; i <= max_y; i++) {
            if ((i & 1) == 0) {
                for (int j = min_x; j <= max_x; j++) {
                    UpdateParticle(IntVector{ j, i });
                }
            }
            else {
                for (int j = max_x; j >= min_x; j--) {
                    UpdateParticle(IntVector{ j, i });
                }
            }
        }
    }

    void IterateTileAlternate() {
        UpdateDraws();
        

        if (simulated_cell_count == 0 && simulated_previous == 0) {
            UpdateZoneRectangle();
            d_rec = d_rec_w;
            return;
        }
        simulated_previous = simulated_cell_count;

        UpdateParticles();

        // Update grid
        for (ParticleUpdate& pu : updates) {
            SwapParticles(pu.source, pu.dest);
        }
        updates.clear();

        UpdateZoneRectangle();
        
    }
};