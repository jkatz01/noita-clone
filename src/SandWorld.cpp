#pragma once

#include <iostream>
#include "raylib.h"
#include "SandTile.cpp"
#include "SandData.h"
#include "NeighbourTD.h"

class SandWorld {
public:
	// Currently the world is just one tile

	int tile_size		 = 100; // must match the SandTile size
	int screen_size		 = 800;
	int scaled_size		 = screen_size / tile_size;
	int tile_width		 = 1;
	int tile_height      = tile_width;
	int scaled_tile_size = scaled_size / tile_width;

	int seed = 7000;

	ParticleType brush_choice = SAND;
	int			 brush_size = 20;

	std::vector<SandTile*> world_tiles; //fixed world size for now, otherwise the indexing can get wrong
	SandTile* first_tile = nullptr;

	SandWorld(int _tile_size, int _screen_size, int _tile_width) {
		tile_size = _tile_size;
		screen_size = _screen_size;
		scaled_size = screen_size / tile_size;
		tile_width = _tile_width;
		tile_height = tile_width;
		scaled_tile_size = scaled_size / tile_width;
	}

	bool MouseInBounds(IntVector pos) {
		return ((pos.x >= 0 && pos.x < screen_size) && (pos.y >= 0 && pos.y < screen_size));
	}

	//TODO: 
	IntVector CursorToWorld(IntVector screen_pos) {
		// Screen mouse position -> Grid scaled mouse position
		if (!MouseInBounds(screen_pos)) {
			return {0, 0};
		}
		IntVector new_pos = { screen_pos.x / scaled_tile_size, screen_pos.y / scaled_tile_size };
		return new_pos;
	}
	IntVector CursorToTile(IntVector grid_pos) {
		// Grid scaled mouse position -> tile position
		grid_pos.x /= tile_size;
		grid_pos.y /= tile_size;
		return grid_pos;
	}
	IntVector CursorToInnerPosition(IntVector grid_pos, IntVector tile_pos) {
		//Grid scaled mouse position + Tile Position -> Mouse position inside tile
		int x = grid_pos.x - (tile_pos.x * tile_size);
		int y = grid_pos.y - (tile_pos.y * tile_size);
		return {x, y};
	}
	void draw() {
		// Draw material at mouse position
	}

	void MakeMultiTileWorld() {
		srand(seed);
		world_tiles.reserve(tile_width * tile_height);
		for (int i = 0; i < tile_width * tile_height; i++) {
			SandTile *t = new SandTile(tile_size, { i % tile_width , i / tile_width });
			world_tiles.push_back(t);
			world_tiles.back()->AddMaterialSquare( { 90, 90 }, 30, SAND);
		}
		// update each tile with all its neighbours
		// maybe just do this naively?
		MultiWorldAddNeighbours();
	}

	void MultiWorldAddNeighbours() {
		for (int i = 0; i < tile_width * tile_height; i++) {
			int min_range = i - tile_width - 1;
			int max_range = i + tile_width + 1;
			if (min_range < 0) min_range = 0; 
			if (max_range >= tile_width * tile_height) max_range = tile_width * tile_height - 1;
			for (int j = min_range; j <= max_range; j++) {
				NeighbourTD n_index = NeighbourIndexFromTilePosition(world_tiles[i]->position, world_tiles[j]->position);
				if (n_index != ND_MYSELF) {
					world_tiles[i]->tile_neighbours[n_index] = world_tiles[j];
				}
			}
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
		}
	}

	void DrawTileBoundaries() {
		for (SandTile* tile : world_tiles) {
			DrawRectangleLines(
				(tile->position.x * tile_size) * scaled_tile_size,
				(tile->position.y * tile_size) * scaled_tile_size,
				tile_size * scaled_tile_size, tile_size * scaled_tile_size, RED);
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

		UpdateMultiTileWorld();
		DrawMultiTileWorld();
		//DrawTileBoundaries();

		DrawFps();
		DrawInfoStuff(BLACK);
	}
	
	void DrawFps() {
		char fps_msg[8];
		_itoa_s(GetFPS(), fps_msg, 8, 10);
		DrawText(fps_msg, 600, 50, 20, BLACK);
	}

	SandTile* GetTileFromPos(IntVector tile_pos) {
		int tile_index = tile_pos.x % tile_width + tile_pos.y * tile_width;
		if (world_tiles[tile_index] != nullptr) {
			return world_tiles[tile_index];
		}
		return nullptr;
	}

	void DrawInfoStuff(Color col) {
		IntVector scaled_pos = CursorToWorld({ GetMouseX(), GetMouseY() });
		IntVector tile_pos = CursorToTile(scaled_pos);

		char mouse_pos[100];
		snprintf(mouse_pos, 100, "X: %d, Y: %d", GetMouseX(), GetMouseY());
		DrawText(mouse_pos, 50, 50, 20, col);

		char mouse_pos_s[100];
		snprintf(mouse_pos_s, 100, "X: %d, Y: %d", scaled_pos.x, scaled_pos.y);
		DrawText(mouse_pos_s, 50, 80, 20, col);

		char tile[20];
		snprintf(tile, 20, "Tile: %d, %d", tile_pos.x, tile_pos.y);
		DrawText(tile, 50, 110, 20, col);

		char brush[10];
		snprintf(brush, 10, param_ref[brush_choice].type_name.c_str(), 1);
		DrawText(brush, 50, 140, 20, col);

		DrawCircleLines((int)GetMouseX(), (int)GetMouseY(), brush_size, col);
	}

	void AddParticles() {
		if (!MouseInBounds({ GetMouseX(), GetMouseY() })) {
			return;
		}
		IntVector scaled_pos = CursorToWorld({ GetMouseX(), GetMouseY() });
		IntVector tile_pos = CursorToTile(scaled_pos);
		IntVector inner_pos = CursorToInnerPosition(scaled_pos, tile_pos);

		SandTile *tile = GetTileFromPos(tile_pos);

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
			tile->AddMaterialCircle(inner_pos, brush_size, brush_choice);
		}
		else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			tile->DeleteMaterialCircle(inner_pos, brush_size);
		}
	}
};
