#pragma once

#include <cstdio>
#include <iostream>
#include <array>
#include <format>
#include "raylib.h"
#include "SandTile.cpp"
#include "SandData.h"
#include "NeighbourTD.h"

struct DebugFlags {
	bool tileBoundaries;
	bool emptyTiles;
	bool dirtyRecs;
};

class SandWorld {
public:
	// Currently the world is just one tile
	int world_width = 100;
	int world_height = 100;

	int tile_size		= 100;
	int tiles_width		= 1;
	int tiles_height	= 1;

	int tile_number		= tiles_width * tiles_height;
	Camera2D *camera; //TODO: maybe the camera should be kept inside the SandWorld?
	Rectangle gui_bounds = {0};

	size_t seed = 7000;
	size_t frame_counter = 0;

	ParticleType brush_choice = SAND;
	int			 brush_size = 2;

	std::vector<SandTile*> world_tiles; //fixed world size for now, otherwise the indexing can get wrong
	std::vector<Color*> tile_color_buffers;

	DebugFlags *debug_flags;

	// TODO: tile size should be a power of 2, not something custom decided by the world size
	SandWorld(int _tiles_horizontal, int _tiles_vertical, int _tile_size, Camera2D *cam) {
		world_width = _tile_size * _tiles_horizontal;
		world_height = _tile_size * _tiles_vertical;
		tiles_width = _tiles_horizontal;
		tiles_height = _tiles_vertical;


		tile_size = _tile_size;

		tile_number = tiles_width * tiles_height;
		camera = cam;
		world_tiles.reserve(tile_number);
		tile_color_buffers.reserve(tile_number);
	}

	bool MouseInBounds(IntVector pos) {
		return ( (pos.x >= 0 && pos.x < GetScreenWidth()) && (pos.y >= 0 && pos.y < GetScreenHeight()) );
	}

	bool MouseInGuiBounds(IntVector pos) {
		return ( (pos.x >= gui_bounds.x && pos.x < gui_bounds.x + gui_bounds.width) && (pos.y >= gui_bounds.y && pos.y < gui_bounds.y + gui_bounds.height) );
	}

	// Screen mouse position -> Grid scaled mouse position
	IntVector CursorToWorld(IntVector screen_pos) {
		// potentially GetScreenToWorld2D ??
		Vector2 the = GetScreenToWorld2D(screen_pos.toVector2(), *camera);
		screen_pos = {(int)the.x, (int)the.y};
		
		if (screen_pos.x < 0) screen_pos.x = 0;
		if (screen_pos.y < 0) screen_pos.y = 0;
		if (screen_pos.x >= world_width ) screen_pos.x = world_width - 1;
		if (screen_pos.y >= world_height ) screen_pos.y = world_height - 1;

		IntVector new_pos = { (int)(screen_pos.x ), (int)(screen_pos.y ) };
		return new_pos;
	}

	// Grid scaled mouse position -> tile position
	IntVector CursorToTile(IntVector grid_pos) {
		grid_pos.x /= tile_size;
		grid_pos.y /= tile_size;

		if (grid_pos.x >= tiles_width) grid_pos.x = tiles_width - 1;
		if (grid_pos.y >= tiles_height) grid_pos.y = tiles_height - 1;

		return grid_pos;
	}

	//Grid scaled mouse position + Tile Position -> Mouse position inside tile
	IntVector CursorToInnerPosition(IntVector grid_pos, IntVector tile_pos) {
		int x = grid_pos.x - (tile_pos.x * tile_size);
		int y = grid_pos.y - (tile_pos.y * tile_size);
		return {x, y};
	}

	bool CursorInWorldBounds(IntVector pos) {
		return ((pos.x >= 0 && pos.x < tile_size*tiles_width) && (pos.y >= 0 && pos.y < tile_size*tiles_height));
	}

	void draw() {
		//Draw material at mouse position
	}

	IntVector VectorFromIndex(int index) {
		return {index % tile_size, index / tile_size };
	}

	void MakeMultiTileWorld() {
		srand((unsigned int)seed);
		for (int i = 0; i < tile_number; i++) {
			SandTile *t = new SandTile(tile_size, { i % tiles_width , i / tiles_width });
			world_tiles.push_back(t);
			//t->AddMaterialSquare({50, 50}, 2, SAND);
		}
		
		MultiWorldAddNeighbours();
	}

	void MultiWorldAddNeighbours() {
		for (int i = 0; i < tile_number; i++) {
			int min_range = i - tiles_width - 1;
			int max_range = i + tiles_width + 1;
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

	//TODO: only unload/update tiles that have changed since the last frame
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
			DrawTexture(textures.back(), (world_tiles[i]->position.x * tile_size), (world_tiles[i]->position.y * tile_size), WHITE);
		}
		
	}

	void DrawTileBoundaries() {
		if (debug_flags->tileBoundaries) {
			for (SandTile* tile : world_tiles) {
				DrawRectangleLines(
					(tile->position.x * tile_size),
					(tile->position.y * tile_size),
					tile_size, tile_size, RED);
			}
		}
	}

	void DrawEmptyTiles() {
		if (debug_flags->emptyTiles) {
			for (SandTile* tile : world_tiles) {
				if (tile->simulated_cell_count == 0) {
					DrawRectangle(
						(tile->position.x * tile_size),
						(tile->position.y * tile_size),
						tile_size , tile_size , Color{ 0, 0, 0, 150 });
				}
			}
		}
	}

	void DrawDirtyRecs() {
		if (debug_flags->dirtyRecs) {
			for (SandTile* tile : world_tiles) {
				if (tile->simulated_cell_count != 0) {
					DrawRectangleLines(
						(tile->position.x * tile_size + tile->d_rec.min.x) ,
						(tile->position.y * tile_size + tile->d_rec.min.y) ,
						(tile->d_rec.max.x - tile->d_rec.min.x + 1) ,
						(tile->d_rec.max.y - tile->d_rec.min.y + 1) ,
						GREEN);
				}
			}
		}
	}

	void executeFrame() {
		BrushSettings();
		BrushInput();

		UpdateMultiTileWorld();
		DrawTileImages();

		DrawEmptyTiles();
		DrawTileBoundaries();
		DrawDirtyRecs();
		

		frame_counter++;
	}
	
	void DrawFps(Color col, Font font) {
		std::string m1 = std::format("FPS: {}", GetFPS());
		DrawTextEx(font, m1.c_str(), {600, 50}, font.baseSize, 0, col);

		std::string m2 = std::format("FT: {:.2f}", GetFrameTime() * 1000);
		DrawTextEx(font, m2.c_str(), { 600, float(50 + font.baseSize) }, font.baseSize, 0, col);
	}

	SandTile* GetTileFromPos(IntVector tile_pos) {
		if (tile_pos.x >= tiles_width) tile_pos.x = tiles_width - 1;
		if (tile_pos.y >= tiles_height) tile_pos.y = tiles_height - 1;

		int tile_index = tile_pos.x % tiles_width + tile_pos.y * tiles_width;
		if (world_tiles[tile_index] != nullptr) {
			return world_tiles[tile_index];
		}
		return nullptr;
	}

	void DrawInfoStuff(Color col, Font font) {
		IntVector scaled_pos = CursorToWorld({ GetMouseX(), GetMouseY() });
		IntVector tile_pos = CursorToTile(scaled_pos);
		IntVector inner_pos = CursorToInnerPosition(scaled_pos, tile_pos);
		SandTile* tile = GetTileFromPos(tile_pos);

		std::string m1 = std::format("Mouse:  X: {}, Y: {}", GetMouseX(), GetMouseY());
		std::string m2 = std::format("Scaled: X: {}, Y: {}", scaled_pos.x, scaled_pos.y);
		std::string m3 = std::format("Tile:   X: {}, Y: {}", inner_pos.x, inner_pos.y);
		std::string m4 = std::format("Brush: {}", param_ref[brush_choice].type_name);
		std::string m5 = std::format("Cell count: {}", tile->simulated_cell_count);
		std::string m6 = std::format("Vel: X: {}, Y: {}", 
			tile->grid[tile->index(inner_pos)].velocity.x,
			tile->grid[tile->index(inner_pos)].velocity.y);

		PrintOnScreen(col, font, {m1, m2, m3, m4, m5, m6});
		DrawCircleLines((int)GetMouseX(), (int)GetMouseY(), (brush_size+1) / 2.0 * camera->zoom, col);
	}

	void AddMaterialSingleInWorld(IntVector world_pos, ParticleType m_type) {
		if (CursorInWorldBounds(world_pos)) {
			IntVector tile_pos = CursorToTile(world_pos);
			IntVector inner_pos = CursorToInnerPosition(world_pos, tile_pos);

			if (m_type == EMPTY) {
				GetTileFromPos(tile_pos)->DeleteMaterialSingle(inner_pos);
			}
			else {
				// random chance
				if (rand_int(0, 5) < 4) {
					return;
				}
				GetTileFromPos(tile_pos)->AddMaterialSingle(inner_pos, m_type);
			}
		}
	}

	void AddMaterialLineInWorld(IntVector pos, IntVector prev_pos, int size, ParticleType m_type) {
		// returns lists of tiles to call and where in the tile to add
		IntVector min = { std::min(pos.x, prev_pos.x) - size , std::min(pos.y, prev_pos.y) - size };
		IntVector max = { std::max(pos.x, prev_pos.x) + size , std::max(pos.y, prev_pos.y) + size};

		for (int i = min.y; i < max.y; i++) {
			for (int j = min.x; j < max.x; j++) {
				if (CheckCollisionPointLine({ (float)j, (float)i }, pos.toVector2(), prev_pos.toVector2(), size)) {
					// include
					IntVector scaled_pos = { j, i };
					AddMaterialSingleInWorld(scaled_pos, m_type);
				}
			}
		}
	}

	void AddMaterialCircleInWorld(IntVector pos, int diameter, ParticleType m_type) {
		if (diameter == 1) {
			IntVector scaled_pos = { pos.x, pos.y };
			AddMaterialSingleInWorld(scaled_pos, m_type);
		}
		diameter = diameter / 2;
		for (int i = -diameter; i < diameter; i++) {
			for (int j = -diameter; j < diameter; j++) {
				if (i * i + j * j <= diameter * diameter) {
					IntVector scaled_pos = { j + pos.x, i + pos.y };
					AddMaterialSingleInWorld(scaled_pos, m_type);
				}
			}
		}
	}

	void BrushInput() {
		if (!MouseInBounds({ GetMouseX(), GetMouseY() })) {
			return;
		}
		IntVector scaled_pos = CursorToWorld({ GetMouseX(), GetMouseY() });
		IntVector tile_pos = CursorToTile(scaled_pos);
		SandTile* tile = GetTileFromPos(tile_pos);

		static int mouse_held = 0;
		static IntVector prev_pos = scaled_pos;

		if ( !CursorInWorldBounds(scaled_pos) || MouseInGuiBounds({ GetMouseX(), GetMouseY() }) ) {
			mouse_held = 0;
			return;
		}
		Vector2 the = GetScreenToWorld2D({ (float)GetMouseX(), (float)GetMouseY() }, *camera);
		if (the.x < 0 || the.y < 0) {
			mouse_held = 0;
			return;
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			AddMaterialCircleInWorld(scaled_pos, brush_size, brush_choice);
			if (mouse_held) {
				AddMaterialLineInWorld(scaled_pos, prev_pos, brush_size/2, brush_choice);
			}
			prev_pos = scaled_pos;
			mouse_held = 1;
		}
		else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			// Delete
			AddMaterialCircleInWorld(scaled_pos, brush_size, EMPTY);
		}
		else {
			mouse_held = 0;
		}
	}

	void BrushSettings() {
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
			if (frame_counter % 5 == 0) {
			}
			brush_size += 10;

		}
		else if (IsKeyPressed(KEY_E)) {
			brush_size -= 10;
			if (brush_size < 2) {
				brush_size = 2;
			}
		}
	}

	void SetBrushMaterial(ParticleType type) {
		brush_choice = type;
	}
	void SetBrushSize(int size) {
		if (size < 2) {
			size = 2;
		}
		brush_size = size;
	}

	void PrintOnScreen(Color col, Font font, std::vector<std::string> messages) {
		float h = float(font.baseSize);
		for (std::string m : messages) {
			DrawTextEx(font, m.c_str(), {50, 50 + h}, font.baseSize, 0, col);
			h += font.baseSize;
		}
	}
};
