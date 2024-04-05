#include <stdlib.h>
#include "raylib.h"

typedef struct {
	Color color;
} THING;

int main() {
	int world_size = 800;
	InitWindow(world_size, world_size, "World");
	SetTargetFPS(60);

	THING *world = (THING*)malloc(world_size * world_size * sizeof(THING));
	for (int i = 0; i < world_size*world_size; i++) {
		world[i].color = RED;
	}
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);
		for (int i = 0; i < world_size*world_size; i++) {
			DrawPixel(i % world_size , i / world_size, world[i].color);
		}
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
