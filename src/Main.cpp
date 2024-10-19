#include <iostream>
#include "raylib.h"
#include "SandWorld.cpp"

SandWorld* world;

int main() {
	world = new SandWorld();

	InitWindow(world->screen_size, world->screen_size, "World");
	//SetTargetFPS(60);
	
	world->MakeOneTileWorld();

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);

		world->executeFrame();

		DrawCircleLines((int)GetMouseX(), (int)GetMouseY(), 40, BLACK);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}
