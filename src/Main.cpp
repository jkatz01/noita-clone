#include <iostream>
#include "raylib.h"
#include "SandWorld.cpp"
#include "CameraController.cpp"

int main() {
	const int screen_width = 1200;
	const int screen_height = 800;
	const int world_width = 600;
	const int world_height = 300;

	InitWindow(screen_width, screen_height, "World");
	SetTraceLogLevel(LOG_WARNING);
	SetTargetFPS(60);

	CameraController world_cam(screen_width, screen_height, world_width, world_height);
	SandWorld world(world_width, world_height, 6, &world_cam.camera);

	world.MakeMultiTileWorld();
	world.AllocateImageTileBuffers();

	Font font = LoadFontEx("assets/PERB____.ttf", 48, 0, 0);
	SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);

	Image buddyworld = LoadImage("assets/beautifu.png");
	ImageResize(&buddyworld, world_width, world_height); 
	Texture2D bg_texture = LoadTextureFromImage(buddyworld);
	UnloadImage(buddyworld);

	

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);

		world_cam.MoveCamera();

		BeginMode2D(world_cam.camera);

			DrawTexture(bg_texture, 0, 0, WHITE);

			world.executeFrame();
			
		EndMode2D();

		world.DrawFps({245, 50, 180, 255}, font);
		world.DrawInfoStuff({ 245, 50, 180, 255 }, font);
		//DrawTextEx(font, "PRESS R TO MORE SAND", {500.0f, 500.0f}, font.baseSize,0, WHITE);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}
