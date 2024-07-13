#include <iostream>
#include "raylib.h"
#include "SandWorld.cpp"

SandWorld* world;




int main() {
	world = new SandWorld();

	InitWindow(world->screen_size, world->screen_size, "World");
	//SetTargetFPS(0);
	

	world->make_one_chunk_world();

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);

		world->executeFrame();

		DrawCircleLines((int)GetMouseX(), (int)GetMouseY(), 40, BLACK);

		char fps_msg[8];
		_itoa_s(GetFPS(), fps_msg, 8, 10);
		DrawText(fps_msg, 600, 160, 40, BLACK);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
