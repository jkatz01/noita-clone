#pragma once
#include "IntVector.h"

// Neighbour Tile Direction
enum NeighbourTD {
    ND_UP = 0,
    ND_UP_RIGHT = 1,
    ND_RIGHT = 2,
    ND_DOWN_RIGHT = 3,
    ND_DOWN = 4,
    ND_DOWN_LEFT = 5,
    ND_LEFT = 6,
    ND_UP_LEFT = 7,
    ND_MYSELF = 1000
};


NeighbourTD NeighbourIndexFromTilePosition(IntVector tile_origin, IntVector tile_n);
NeighbourTD NeighbourFromPosition(IntVector pos, int tile_size);
IntVector   TranslateParticleToNeighbour(IntVector pos, int tile_size); // Translates out of bounds position from self to appropriate position in neighbouring tile
IntVector   VecDifference(IntVector vec1, IntVector vec2);



