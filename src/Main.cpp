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
	if (GuiButton(CLITERAL(Rectangle) { screen_width - 200, 100, 100, 50 }, "SAND")) world.SetBrushMaterial(SAND);
	if (GuiButton(CLITERAL(Rectangle) { screen_width - 100, 100, 100, 50 }, "WATER")) world.SetBrushMaterial(WATER);
	if (GuiButton(CLITERAL(Rectangle) { screen_width - 200, 150, 100, 50 }, "STONE")) world.SetBrushMaterial(STONE);
	if (GuiButton(CLITERAL(Rectangle) { screen_width - 100, 150, 100, 50 }, "STEAM")) world.SetBrushMaterial(STEAM);

	GuiSlider({ screen_width - 200, 50, 200, 50 }, "BRUSH", "", &bsize, 2, 100);
	if (prev_bsize != bsize) world.SetBrushSize((int)bsize);
	if (world.brush_size != bsize) bsize = world.brush_size;
	prev_bsize = bsize;
}

void ChangeGuiFontSize(int size) {
	Font gf = GuiGetFont();
	gf.baseSize = size;
	GuiSetFont(gf);
}

int main() {
	InitWindow(screen_width, screen_height, "World");
	SetTraceLogLevel(LOG_WARNING);
	SetTargetFPS(5);

	CameraController world_cam(screen_width, screen_height, world_width, world_height);
	
	SandWorld world(world_width, world_height, 6, &world_cam.camera);
	world.MakeMultiTileWorld();
	world.AllocateImageTileBuffers();
	world.gui_bounds = { screen_width - 200, 50, 200, 150 };

	Image buddyworld = LoadImage("assets/beautifu.png");
	ImageResize(&buddyworld, world_width, world_height); 
	Texture2D bg_texture = LoadTextureFromImage(buddyworld);
	UnloadImage(buddyworld);

	GuiLoadStyle("assets/style_cherry.rgs");
	ChangeGuiFontSize(7);

	Font font = LoadFontEx("assets/Westington.ttf", 48, 0, 0);
	SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);

		world_cam.MoveCamera();

		BeginMode2D(world_cam.camera);
			DrawTexture(bg_texture, 0, 0, WHITE);
			world.executeFrame();
		EndMode2D();

		world.DrawFps({225, 115, 115, 255}, font);
		world.DrawInfoStuff({ 225, 115, 115, 255 }, font);
		//DrawTextEx(font, "PRESS R TO MORE SAND", {500.0f, 500.0f}, font.baseSize,0, WHITE);

		WorldDrawGui(world);
		
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
