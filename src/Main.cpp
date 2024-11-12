#include <iostream>
#include "raylib.h"
#include "SandWorld.cpp"
#include "CameraController.cpp"

#define RAYGUI_IMPLEMENTATION 
#pragma warning( disable : 4996 4267 )
#include "raygui.h"
#pragma warning( default : 4996 4267 )

const int screen_width = 1400;
const int screen_height = 1000;
const int world_width = 600;
const int world_height = 300;

void WorldDrawGui(SandWorld &world) {
	// slider
	static float prev_bsize = 2, bsize = 2;
	if (GuiButton(CLITERAL(Rectangle) { screen_width - 200, 100, 100, 50 }, "Sand")) world.SetBrushMaterial(SAND);
	if (GuiButton(CLITERAL(Rectangle) { screen_width - 100, 100, 100, 50 }, "Water")) world.SetBrushMaterial(WATER);
	if (GuiButton(CLITERAL(Rectangle) { screen_width - 200, 150, 100, 50 }, "Stone")) world.SetBrushMaterial(STONE);
	if (GuiButton(CLITERAL(Rectangle) { screen_width - 100, 150, 100, 50 }, "Steam")) world.SetBrushMaterial(STEAM);

	GuiSlider({ screen_width - 200, 50, 200, 50 }, "brush", "", &bsize, 2, 100); //TODO: make slider change if E/R pressed
	if (prev_bsize != bsize) world.SetBrushSize((int)bsize);
	prev_bsize = bsize;
}

int main() {
	InitWindow(screen_width, screen_height, "World");
	SetTraceLogLevel(LOG_WARNING);
	SetTargetFPS(60);

	CameraController world_cam(screen_width, screen_height, world_width, world_height);
	
	SandWorld world(world_width, world_height, 6, &world_cam.camera);
	world.MakeMultiTileWorld();
	world.AllocateImageTileBuffers();
	world.gui_bounds = { screen_width - 200, 50, 200, 150 };

	Font font = LoadFontEx("assets/PER_____.ttf", 48, 0, 0);
	SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);

	Image buddyworld = LoadImage("assets/beautifu.png");
	ImageResize(&buddyworld, world_width, world_height); 
	Texture2D bg_texture = LoadTextureFromImage(buddyworld);
	UnloadImage(buddyworld);

	Font guifont = GetFontDefault();
	guifont.baseSize = 4;
	GuiSetFont(guifont);

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

		WorldDrawGui(world);
		
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
