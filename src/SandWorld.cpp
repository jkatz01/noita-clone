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
	IntVector CursorToWorld(IntVector pos) {
		// Mouse position to position in world grid
		IntVector new_pos = {pos.x / scaled_size, pos.y / scaled_size};
		return new_pos;
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
		first_chunk->AddMaterialSquare(IntVector {50, 10}, 10, SAND);
	}

	void UpdateOneTileWorld() {
		first_chunk->IterateTileAlternate();
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
		AddParticles();
		UpdateOneTileWorld();
		DrawOneTileWorld();
		DrawFps();
	}
	
	void DrawFps() {
		char fps_msg[8];
		_itoa_s(GetFPS(), fps_msg, 8, 10);
		DrawText(fps_msg, 600, 50, 40, BLACK);
	}

	void AddParticles() {
		IntVector scaled_pos = CursorToWorld({ GetMouseX(), GetMouseY() });

		char mouse_pos[100];
		snprintf(mouse_pos, 100, "X: %d, Y: %d", GetMouseX(), GetMouseY());
		DrawText(mouse_pos, 50, 50, 40, BLACK);

		char mouse_pos_s[100];
		snprintf(mouse_pos_s, 100, "X: %d, Y: %d", scaled_pos.x, scaled_pos.y);
		DrawText(mouse_pos_s, 50, 100, 40, BLACK);

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			first_chunk->AddMaterialSquare(scaled_pos, 10, SAND);
		}
		else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			first_chunk->AddMaterialSquare(scaled_pos, 10, EMPTY);
		}
	}
};
