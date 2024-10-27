#include <iostream>
#include "raylib.h"
#include "SandWorld.cpp"

SandWorld* world;

int main() {
	world = new SandWorld(200, 800, 4);

	InitWindow(world->screen_size, world->screen_size, "World");
	SetTraceLogLevel(LOG_WARNING);
	SetTargetFPS(60);

	world->MakeMultiTileWorld();
	world->AllocateImageTileBuffers();

	Font font = LoadFontEx("assets/PERB____.ttf", 48, 0, 0);
	SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);

	Image buddyworld = LoadImage("assets/beautifu.png");
	//ImageCrop(&buddyworld, (Rectangle){ 100, 10, 280, 380 });      // Crop an image piece
	//ImageFlipHorizontal(&buddyworld);                              // Flip cropped image horizontally
	ImageResize(&buddyworld, 1200, 1200);                            // Resize flipped-cropped image
	Texture2D bg_texture = LoadTextureFromImage(buddyworld);
	UnloadImage(buddyworld);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);

		//DrawTexture(bg_texture, 0, 0, WHITE);

		world->executeFrame();

		world->DrawFps({245, 50, 180, 255}, font);
		world->DrawInfoStuff({ 245, 50, 180, 255 }, font);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}
