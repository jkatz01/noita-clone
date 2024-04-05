#include <stdlib.h>
#include "raylib.h"

#define LIGHTBLUE ColorFromHSV(200, 0.78, 0.92)

enum PixelType {
	DIRT,
	AIR
};

typedef struct {
	enum PixelType type;
} Pixel;

void generate_world(Pixel **world, int world_size) {
	for (int i = 0; i < world_size/2; i++) {
		for (int j = 0; j < world_size; j++) {
			world[i][j].type = AIR;	
		}
	}
	for (int i = world_size/2; i < world_size; i++) {
		for (int j = 0; j < world_size; j++) {
			world[i][j].type = DIRT;	
		}
	}
}

int main() {
	int world_size = 800;
	InitWindow(world_size, world_size, "World");
	SetTargetFPS(60);
	Color color_lookup[2] = {DARKBROWN, LIGHTBLUE};
	
	Pixel *_world = (Pixel*)malloc(world_size * world_size * sizeof(Pixel));
	if (!_world) return -1;
	Pixel **world = (Pixel**)malloc(world_size * sizeof(Pixel*));
	if (!world) {
		free(_world);
		return -1;
	}
	for (int i = 0; i < world_size; i++) {
		world[i] = &(_world[i*world_size]);
	}

	// World Generation
	generate_world(world, world_size);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);
		int x;
		int y;
		for (int i = 0; i < world_size*world_size; i++) {
			x = i % world_size;
			y = i / world_size;
			DrawPixel(x, y, color_lookup[world[y][x].type]);
		}
		EndDrawing();
	}
	CloseWindow();
	free(world);
	return 0;
}
