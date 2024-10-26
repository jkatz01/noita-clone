#include <iostream>
#include "raylib.h"
#include "SandWorld.cpp"

SandWorld* world;

int main() {
	world = new SandWorld(200, 800, 8);

	InitWindow(world->screen_size, world->screen_size, "World");
	SetTraceLogLevel(LOG_WARNING);
	SetTargetFPS(10);

	world->MakeMultiTileWorld();
	world->AllocateImageTileBuffers();

	Font font = LoadFontEx("assets/PERB____.ttf", 48, 0, 0);
	SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);

		

		world->executeFrame();

		world->DrawFps({245, 50, 180, 255}, font);
		world->DrawInfoStuff({ 245, 50, 180, 255 }, font);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}
