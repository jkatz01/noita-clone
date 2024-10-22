#pragma once

#include <iostream>
#include "raylib.h"
#include "SandTile.cpp"
#include "SandData.h"

class SandWorld {
public:
	// Currently the world is just one chunk

	const int chunk_size = 400; // must match the SandTile size
	const int screen_size = 800;
	const int scaled_size = screen_size / chunk_size;

	int seed = 7000;

	ParticleType brush_choice = SAND;
	int			 brush_size = 20;

	SandTile* first_chunk = nullptr;

	//TODO: 
	IntVector CursorToWorld(IntVector pos) {
		// Mouse position to position in world grid
		IntVector new_pos = {pos.x / scaled_size, pos.y / scaled_size};
		return new_pos;
	}
	void CursorToChunk() {
		// Mouse position to tile and position in tile
	}
	void draw() {
		// Draw material at mouse position
	}

	void MakeOneTileWorld() {
		srand(seed);
		first_chunk = new SandTile(chunk_size);
		first_chunk->AddMaterialSquare(IntVector {90, 90}, 30, SAND);
	}

	void UpdateOneTileWorld() {
		first_chunk->IterateTileAlternate();
	}

	void DrawOneTileWorld() {
		for (int i = 0; i < chunk_size; i++) {
			for (int j = 0; j < chunk_size; j++) {
				DrawRectangle(i * scaled_size, j * scaled_size, scaled_size, scaled_size, 
								(first_chunk->grid[first_chunk->index(i, j)].colour));
			}
		}
	}


	void executeFrame() {
		AddParticles();
		UpdateOneTileWorld();
		DrawOneTileWorld();
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
			first_chunk->AddMaterialCircle(scaled_pos, brush_size, brush_choice);
		}
		else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			first_chunk->DeleteMaterialCircle(scaled_pos, brush_size);
		}
	}
};
