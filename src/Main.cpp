#include <iostream>
#include "raylib.h"
#include "SandWorld.cpp"
#include "CameraController.cpp"

int main() {
	const int screen_width = 1920;
	const int screen_height = 920;

	InitWindow(screen_width, screen_height, "World");
	SetTraceLogLevel(LOG_WARNING);
	//SetTargetFPS(60);

	CameraController world_cam(screen_width, screen_height);

	SandWorld world(640, screen_width, screen_height, 8, 4, &world_cam.camera);
	world.MakeMultiTileWorld();
	world.AllocateImageTileBuffers();

	Font font = LoadFontEx("assets/PERB____.ttf", 48, 0, 0);
	SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);

	Image buddyworld = LoadImage("assets/buddyworld.png");
	//ImageCrop(&buddyworld, (Rectangle){ 100, 10, 280, 380 });      // Crop an image piece
	//ImageFlipHorizontal(&buddyworld);                              // Flip cropped image horizontally
	ImageResize(&buddyworld, screen_width, screen_height);                            // Resize flipped-cropped image
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

		EndDrawing();
	}
	CloseWindow();
	return 0;
}
