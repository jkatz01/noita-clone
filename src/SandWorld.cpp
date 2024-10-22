#pragma once

#include <iostream>
#include "raylib.h"
#include "SandTile.cpp"
#include "SandData.h"

class SandWorld {
public:
	// Currently the world is just one tile

	int tile_size		 = 100; // must match the SandTile size
	int screen_size		 = 800;
	int scaled_size		 = screen_size / tile_size;
	int tile_width		 = 1;
	int scaled_tile_size = scaled_size / tile_width;

	int seed = 7000;

	ParticleType brush_choice = SAND;
	int			 brush_size = 20;

	std::vector<SandTile*> world_tiles;
	SandTile* first_tile = nullptr;

	SandWorld(int _tile_size, int _screen_size, int _tile_width) {
		tile_size = _tile_size;
		screen_size = _screen_size;
		scaled_size = screen_size / tile_size;
		tile_width = _tile_width;
		scaled_tile_size = scaled_size / tile_width;
	}

	//TODO: 
	IntVector CursorToWorld(IntVector pos) {
		// Mouse position to position in world grid
		IntVector new_pos = {pos.x / scaled_size, pos.y / scaled_size};
		return new_pos;
	}
	void CursorTotile() {
		// Mouse position to tile and position in tile
	}
	void draw() {
		// Draw material at mouse position
	}

	void MakeMultiTileWorld() {
		srand(seed);
		world_tiles.reserve(tile_width);
		for (int i = 0; i < tile_width; i++) {
			SandTile *t = new SandTile(tile_size, { i, i / tile_width });
			world_tiles.push_back(t);
			world_tiles.back()->AddMaterialSquare( { 90, 90 }, 30, SAND);
		}
	}

	void UpdateMultiTileWorld() {
		for (SandTile* tile : world_tiles) {
			tile->IterateTileAlternate();
		}
	}

	void DrawMultiTileWorld() {
		for (SandTile* tile : world_tiles) {
			for (int i = 0; i < tile_size; i++) {
				for (int j = 0; j < tile_size; j++) {
					DrawRectangle(
						(tile->position.x * tile_size + i) * scaled_tile_size,
						(tile->position.y * tile_size + j) * scaled_tile_size,
						scaled_tile_size, scaled_tile_size, (tile->grid[tile->index(i, j)].colour) );
				}
			}
			DrawRectangle(
				(tile->position.x * tile_size) * scaled_tile_size,
				(tile->position.y * tile_size) * scaled_tile_size,
				scaled_tile_size, scaled_size, RED);
		}
	}

	void MakeOneTileWorld() {
		srand(seed);
		first_tile = new SandTile(tile_size, {0, 0});
		first_tile->AddMaterialSquare( {90, 90}, 30, SAND);
	}

	void UpdateOneTileWorld() {
		first_tile->IterateTileAlternate();
	}

	void DrawOneTileWorld() {
		for (int i = 0; i < tile_size; i++) {
			for (int j = 0; j < tile_size; j++) {
				DrawRectangle(i * scaled_size, j * scaled_size, scaled_size, scaled_size, 
								(first_tile->grid[first_tile->index(i, j)].colour));
			}
		}
	}


	void executeFrame() {
		AddParticles();

		//UpdateOneTileWorld();
		//DrawOneTileWorld();
		UpdateMultiTileWorld();
		DrawMultiTileWorld();

		DrawFps();
		DrawInfoStuff(BLACK);
	}
	
	void DrawFps() {
		char fps_msg[8];
		_itoa_s(GetFPS(), fps_msg, 8, 10);
		DrawText(fps_msg, 600, 50, 20, BLACK);
	}

	void DrawInfoStuff(Color col) {
		IntVector scaled_pos = CursorToWorld({ GetMouseX(), GetMouseY() });

		char mouse_pos[100];
		snprintf(mouse_pos, 100, "X: %d, Y: %d", GetMouseX(), GetMouseY());
		DrawText(mouse_pos, 50, 50, 20, col);

		char mouse_pos_s[100];
		snprintf(mouse_pos_s, 100, "X: %d, Y: %d", scaled_pos.x, scaled_pos.y);
		DrawText(mouse_pos_s, 50, 80, 20, col);

		char tile[10];
		snprintf(tile, 10, "Tile: %d", 1);
		DrawText(tile, 50, 110, 20, col);

		char brush[10];
		snprintf(brush, 10, param_ref[brush_choice].type_name.c_str(), 1);
		DrawText(brush, 50, 140, 20, col);

		DrawCircleLines((int)GetMouseX(), (int)GetMouseY(), brush_size, col);
	}

	void AddParticles() {
		IntVector scaled_pos = CursorToWorld({ GetMouseX(), GetMouseY() });

		if (IsKeyPressed(KEY_ONE)) {
			brush_choice = SAND;
		}
		else if (IsKeyPressed(KEY_TWO)) {
			brush_choice = WATER;
		}
		else if (IsKeyPressed(KEY_THREE)) {
			brush_choice = STONE;
		}
		else if (IsKeyPressed(KEY_FOUR)) {
			brush_choice = STEAM;
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			first_tile->AddMaterialCircle(scaled_pos, brush_size, brush_choice);
		}
		else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			first_tile->DeleteMaterialCircle(scaled_pos, brush_size);
		}
	}
};
