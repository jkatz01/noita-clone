#include <iostream>
#include "raylib.h"
#include "SandWorld.cpp"

SandWorld* world;

int main() {
	world = new SandWorld(100, 800, 4);

	InitWindow(world->screen_size, world->screen_size, "World");
	//SetTargetFPS(60);

	world->MakeMultiTileWorld();

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);

		world->executeFrame();

		EndDrawing();
	}
	CloseWindow();
	return 0;
}
