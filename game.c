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

int main() {
	int world_size = 800;
	InitWindow(world_size, world_size, "World");
	SetTargetFPS(60);
	Color color_lookup[2] = {DARKBROWN, LIGHTBLUE};
	
	Pixel *_world = (Pixel*)malloc(world_size * world_size * sizeof(Pixel));
	Pixel **world = (Pixel**)malloc(world_size * sizeof(Pixel*));
	for (int i = 0; i < world_size; i++) {
		world[i] = &(_world[i*world_size]);
	}
	// World Generation
	for (int i = 0; i < world_size*world_size; i++) {
		if (i < (world_size*world_size) / 2) {
			_world[i].type = AIR;
		}
		else {
			_world[i].type = DIRT;
		}
	}


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
