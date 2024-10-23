#include "NeighbourTD.h"

inline bool NTDInBounds(IntVector pos, int tile_size) {
    return ((pos.x >= 0 && pos.x < tile_size) && (pos.y >= 0 && pos.y < tile_size));
}

inline int NTDsignum(int x) {
    return (x > 0) - (x < 0);
}

NeighbourTD NeighbourIndexFromTilePosition(IntVector tile_origin, IntVector tile_n) {
    if (tile_n == tile_origin) {
        return ND_MYSELF;
    }
    if (tile_n == IntVector{ tile_origin.x,     tile_origin.y - 1 })      return ND_UP;
    else if (tile_n == IntVector{ tile_origin.x + 1, tile_origin.y - 1 }) return ND_UP_RIGHT;
    else if (tile_n == IntVector{ tile_origin.x + 1, tile_origin.y })     return ND_RIGHT;
    else if (tile_n == IntVector{ tile_origin.x + 1, tile_origin.y + 1 }) return ND_DOWN_RIGHT;
    else if (tile_n == IntVector{ tile_origin.x,     tile_origin.y + 1 }) return ND_DOWN;
    else if (tile_n == IntVector{ tile_origin.x - 1, tile_origin.y + 1 }) return ND_DOWN_LEFT;
    else if (tile_n == IntVector{ tile_origin.x - 1, tile_origin.y })     return ND_LEFT;
    else if (tile_n == IntVector{ tile_origin.x - 1, tile_origin.y - 1 }) return ND_UP_LEFT;
    else                                                                  return ND_MYSELF;
}

NeighbourTD NeighbourFromPosition(IntVector pos, int tile_size) {
    if (NTDInBounds(pos, tile_size)) {
        return ND_MYSELF;
    }
    if ((pos.x >= 0 && pos.x < tile_size) && pos.y < 0)             return ND_UP;
    else if (pos.x >= tile_size && pos.y < 0)                       return ND_UP_RIGHT;
    else if (pos.x >= tile_size && (pos.y >= 0 && pos.y < tile_size)) return ND_RIGHT;
    else if (pos.x >= tile_size && pos.y >= tile_size)                return ND_DOWN_RIGHT;
    else if ((pos.x >= 0 && pos.x < tile_size) && pos.y >= tile_size) return ND_DOWN;
    else if (pos.x < 0 && pos.y >= tile_size)                       return ND_DOWN_LEFT;
    else if (pos.x < 0 && (pos.y >= 0 && pos.y < tile_size))        return ND_LEFT;
    else if (pos.x < 0 && pos.y < 0)                                return ND_UP_LEFT;
    else                                                            return ND_MYSELF;
}

IntVector TranslateParticleToNeighbour(IntVector pos, int tile_size) {
    int is_x_neg = (NTDsignum(pos.x) & 1);
    int is_y_neg = (NTDsignum(pos.y) & 1);
    int my_x = (tile_size + (pos.x * (is_x_neg))) % tile_size;
    int my_y = (tile_size + (pos.y * (is_y_neg))) % tile_size;
    return {my_x, my_y};
}

