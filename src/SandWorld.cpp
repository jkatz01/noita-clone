#pragma once

#include <iostream>
#include <array>
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
	int tile_number		 = tile_width * tile_height;

	int seed = 7000;

	ParticleType brush_choice = SAND;
	int			 brush_size = 11;

	std::vector<SandTile*> world_tiles; //fixed world size for now, otherwise the indexing can get wrong
	SandTile* first_tile = nullptr;

	std::vector<Color*> tile_color_buffers;

	SandWorld(int _tile_size, int _screen_size, int _tile_width) {
		tile_size = _tile_size;
		screen_size = _screen_size;
		scaled_size = screen_size / tile_size;
		tile_width = _tile_width;
		tile_height = tile_width;
		scaled_tile_size = scaled_size / tile_width;
		tile_number = tile_width * tile_height;

		world_tiles.reserve(tile_number);
		tile_color_buffers.reserve(tile_number);
	}

	bool MouseInBounds(IntVector pos) {
		return ((pos.x >= 0 && pos.x < screen_size) && (pos.y >= 0 && pos.y < screen_size));
	}
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
		//Draw material at mouse position
	}

	IntVector VectorFromIndex(int index) {
		return {index % tile_size, index / tile_size };
	}

	void MakeMultiTileWorld() {
		srand(seed);
		for (int i = 0; i < tile_number; i++) {
			SandTile *t = new SandTile(tile_size, { i % tile_width , i / tile_width });
			world_tiles.push_back(t);
			//t->AddMaterialSquare({50, 50}, 2, SAND);
		}
		
		MultiWorldAddNeighbours();
	}

	void MultiWorldAddNeighbours() {
		for (int i = 0; i < tile_number; i++) {
			int min_range = i - tile_width - 1;
			int max_range = i + tile_width + 1;
			if (min_range < 0) min_range = 0; 
			if (max_range >= tile_number) max_range = tile_number - 1;
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

	// get all buffers
	// make images from those buffers
	// render all images as textures

	// Only call once

	void AllocateImageTileBuffers() {
		for (int i = 0; i < tile_number; i++) {
			Color* c = new Color[tile_size * tile_size];
			tile_color_buffers.push_back(c);
		}
	}

	Image MakeTileImage(SandTile* tile, Color* buffer) {
		for (int i = 0; i < tile_size * tile_size; i++) {
			buffer[i] = tile->grid[i].colour;
		}
		return { buffer, tile_size , tile_size , 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
	}

	void DrawTileImages() {
		static std::vector<Texture> textures;
		for (Texture& t : textures) {
			UnloadTexture(t);
		}
		textures.clear();

		for (int i = 0; i < tile_number; i++) {
			Image img = MakeTileImage(world_tiles[i], tile_color_buffers[i]);
			textures.push_back(LoadTextureFromImage(img));
			IntVector pos = VectorFromIndex(i);
			DrawTextureEx(textures.back(), Vector2{(float)(world_tiles[i]->position.x * tile_size) * scaled_tile_size,
				(float)(world_tiles[i]->position.y * tile_size) * scaled_tile_size},
				0, scaled_tile_size, WHITE);
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

	void DrawEmptyTiles() {
		for (SandTile* tile : world_tiles) {
			if (tile->simulated_cell_count == 0) {
				DrawRectangle(
					(tile->position.x * tile_size) * scaled_tile_size,
					(tile->position.y * tile_size) * scaled_tile_size,
					tile_size * scaled_tile_size, tile_size * scaled_tile_size, Color{ 0, 200, 0, 100 });
			}
		}
	}

	void executeFrame() {
		AddParticles();

		DrawTileBoundaries();
		DrawEmptyTiles();

		UpdateMultiTileWorld();
		DrawTileImages();

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

		char tile_num[20];
		snprintf(tile_num, 20, "Tile: %d, %d", tile_pos.x, tile_pos.y);
		DrawText(tile_num, 50, 110, 20, col);

		char brush[10];
		snprintf(brush, 10, param_ref[brush_choice].type_name.c_str(), 1);
		DrawText(brush, 50, 140, 20, col);

		SandTile* tile = GetTileFromPos(tile_pos);
		char tile_cell_count[10];
		snprintf(tile_cell_count, 10, "%d", tile->simulated_cell_count);
		DrawText(tile_cell_count, 50, 170, 20, col);

		DrawCircleLines((int)GetMouseX(), (int)GetMouseY(), (brush_size+1) / 2 * scaled_tile_size, col);
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

		if (IsKeyPressed(KEY_R)) {
			brush_size += 10;
		}
		else if (IsKeyPressed(KEY_E)) {
			brush_size -= 10;
			if (brush_size < 1) {
				brush_size = 1;
			}
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			tile->AddMaterialCircle(inner_pos, brush_size, brush_choice);
		}
		else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			tile->DeleteMaterialCircle(inner_pos, brush_size);
		}
	}
};
