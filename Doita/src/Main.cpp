#include <iostream>
#include "raylib.h"
#include "SandChunk.cpp"

Color color_lookup(ParticleType type) {
	switch (type) {
	case EMPTY: return CLITERAL(Color) { 0, 0, 0, 100 };
	case STONE: return DARKBROWN;
	case SAND: return YELLOW;
	default: return RED;
	}
}

int main() {
	const int chunk_size = 100; // must match the SandChunk size
	const int screen_size = 800;
	const int scaled_size = screen_size / chunk_size;

	InitWindow(screen_size, screen_size, "World");
	//SetTargetFPS(0);
	int seed = 7000;

	srand(seed);

	SandChunk *first_chunk = new SandChunk();
	first_chunk->add_material_square(10, 10, 10, SAND);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);

		first_chunk->iterate_chunk();

		for (int i = 0; i < chunk_size; i++) {
			for (int j = 0; j < chunk_size; j++) {
				DrawRectangle(i * scaled_size, j * scaled_size, scaled_size, scaled_size, color_lookup(first_chunk->grid[first_chunk->index(i, j)].type ));
			}
		}

		DrawCircleLines((int)GetMouseX(), (int)GetMouseY(), 40, BLACK);

		char fps_msg[8];
		_itoa_s(GetFPS(), fps_msg, 8, 10);
		DrawText(fps_msg, 600, 160, 40, BLACK);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}