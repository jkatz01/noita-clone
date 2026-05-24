#include "raylib.h"
#include "raygui.h"
#include "SandWorld.cpp"
#include "CameraController.cpp"
#include "DebugTypes.hpp"

const int screen_width = 1400;
const int screen_height = 1000;
const int world_width = 8;
const int world_height = 8;
const int tile_size = 128;

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

	// debug
	GuiCheckBox(CLITERAL(Rectangle) { screen_width - 200, 200, 50, 50}, "boundaries", &world.debug_flags->tileBoundaries);
	GuiCheckBox(CLITERAL(Rectangle) { screen_width - 200, 250, 50, 50}, "empty tiles", &world.debug_flags->emptyTiles);
	GuiCheckBox(CLITERAL(Rectangle) { screen_width - 200, 300, 50, 50}, "dirty recs", &world.debug_flags->dirtyRecs);
	GuiCheckBox(CLITERAL(Rectangle) { screen_width - 200, 350, 50, 50}, "is freefalling", &world.debug_flags->isFreefalling);
	GuiCheckBox(CLITERAL(Rectangle) { screen_width - 200, 400, 50, 50}, "update positions", &world.debug_flags->drawUpdates);
}

void ChangeGuiFontSize(int size) {
	Font gf = GuiGetFont();
	gf.baseSize = size;
	GuiSetFont(gf);
}

int main() {
	InitWindow(screen_width, screen_height, "World");
	SetTraceLogLevel(LOG_WARNING);
	SetTargetFPS(0);

	CameraController world_cam(screen_width, screen_height, world_width * tile_size, world_height * tile_size);
	DebugFlags debug_flags{
		.tileBoundaries = true,
		.emptyTiles = true,
		.dirtyRecs = true,
		.isFreefalling = false,
		.drawUpdates = false
	};

	SandWorld world(world_width, world_height, tile_size, &world_cam.camera, &debug_flags);
	world.MakeMultiTileWorld();
	world.AllocateImageTileBuffers();
	world.gui_bounds = { screen_width - 200, 50, 200, 500 };


	Image buddyworld = LoadImage("assets/beautifu.png");
	ImageResize(&buddyworld, world_width * tile_size, world_height * tile_size);
	Texture2D bg_texture = LoadTextureFromImage(buddyworld);
	UnloadImage(buddyworld);

	// address sanitizer doesn't like this
	GuiLoadStyle("assets/style_cherry.rgs");
	ChangeGuiFontSize(7);

	Font font = LoadFontEx("assets/Westington.ttf", 48, 0, 0);
	SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);


	// remove
	world.world_tiles[0]->AddMaterialSingle({4, 11}, WATER);
	world.world_tiles[0]->AddMaterialSingle({4, 12}, WATER);
	world.world_tiles[0]->AddMaterialSingle({6, 15}, WATER);
	// world_cam.camera.zoom = 20.0f;

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
