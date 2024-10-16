#include <iostream>
#include "raylib.h"
#include "SandTile.cpp"

class SandWorld {
public:
	// Currently the world is just one chunk

	const int chunk_size = 100; // must match the SandTile size
	const int screen_size = 800;
	const int scaled_size = screen_size / chunk_size;

	int seed = 7000;

	SandTile* first_chunk = nullptr;

	//TODO: 
	void CursorToWorld() {
		// Mouse position to position in world grid
	}
	void WorldToChunk() {
		// World position to chunk and position in chunk
	}
	void CursorToChunk() {
		// Above two functions combined
	}
	void draw() {
		// Draw material at mouse position
	}

	Color ColorLookup(ParticleType type) {
		switch (type) {
		case EMPTY: return CLITERAL(Color) { 0, 0, 0, 100 };
		case STONE: return DARKBROWN;
		case SAND: return YELLOW;
		default: return RED;
		}
	}

	void MakeOneTileWorld() {
		srand(seed);
		first_chunk = new SandTile();
		first_chunk->AddMaterialSquare(IntVector {10, 10}, 10, SAND);
	}

	void UpdateOneTileWorld() {
		first_chunk->IterateTile();
	}

	void DrawOneTileWorld() {
		for (int i = 0; i < chunk_size; i++) {
			for (int j = 0; j < chunk_size; j++) {
				DrawRectangle(i * scaled_size, j * scaled_size, scaled_size, scaled_size, 
								ColorLookup(first_chunk->grid[first_chunk->index(i, j)].type));
			}
		}
	}


	void executeFrame() {
		inputHandler();
		UpdateOneTileWorld();
		DrawOneTileWorld();
	}


	void inputHandler() {

		if (IsKeyPressed(KEY_ENTER)) {
			first_chunk->AddMaterialSquare(IntVector{ 10, 10 }, 10, SAND);
		}
		else if (IsKeyPressed(KEY_C)) {
			first_chunk->AddMaterialSquare(IntVector{ 10, 10 }, 10, EMPTY);
		}
	}
};
