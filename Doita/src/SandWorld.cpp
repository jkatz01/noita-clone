#include <iostream>
#include "raylib.h"
#include "SandChunk.cpp"

class SandWorld {
public:
	// Currently the world is just one chunk

	const int chunk_size = 100; // must match the SandChunk size
	const int screen_size = 800;
	const int scaled_size = screen_size / chunk_size;

	int seed = 7000;

	SandChunk* first_chunk = nullptr;

	//TODO: 
	void cursor_to_world() {
		// Mouse position to position in world grid
	}
	void world_to_chunk() {
		// World position to chunk and position in chunk
	}
	void cursor_to_chunk() {
		// Above two functions combined
	}
	void draw() {
		// Draw material at mouse position
	}

	Color color_lookup(ParticleType type) {
		switch (type) {
		case EMPTY: return CLITERAL(Color) { 0, 0, 0, 100 };
		case STONE: return DARKBROWN;
		case SAND: return YELLOW;
		default: return RED;
		}
	}

	void make_one_chunk_world() {
		srand(seed);
		first_chunk = new SandChunk();
		first_chunk->add_material_square(10, 10, 10, SAND);
	}

	void update_one_chunk_world() {
		first_chunk->iterate_chunk();
	}

	void draw_one_chunk_world() {
		for (int i = 0; i < chunk_size; i++) {
			for (int j = 0; j < chunk_size; j++) {
				DrawRectangle(i * scaled_size, j * scaled_size, scaled_size, scaled_size, color_lookup(first_chunk->grid[first_chunk->index(i, j)].type));
			}
		}
	}
};
