#include <iostream>
#include "raylib.h"
#include "SandWorld.cpp"

SandWorld* world;

int main() {
	world = new SandWorld(100, 800, 2);

	InitWindow(world->screen_size, world->screen_size, "World");
	SetTraceLogLevel(LOG_WARNING);
	SetTargetFPS(60);

	world->MakeMultiTileWorld();
	world->AllocateImageTileBuffers();

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);

		world->executeFrame();

		EndDrawing();
	}
	CloseWindow();
	return 0;
}
