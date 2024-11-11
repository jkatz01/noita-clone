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

	int world_width		= 100;
	int world_height	= 100;

	int tile_size		= 100;
	int tiles_width		= 1;
	int tiles_height	= tiles_width;

	int tile_number		= tiles_width * tiles_height;
	Camera2D *camera; //TODO: maybe the camera should be kept inside the SandWorld?

	size_t seed = 7000;
	size_t frame_counter = 0;

	ParticleType brush_choice = SAND;
	int			 brush_size = 1;

	std::vector<SandTile*> world_tiles; //fixed world size for now, otherwise the indexing can get wrong
	SandTile* first_tile = nullptr;

	std::vector<Color*> tile_color_buffers;

	

	SandWorld(int _world_width, int _world_height, int _tiles_width, Camera2D *cam) {
		world_width = _world_width;
		world_height = _world_height;
		tiles_width = _tiles_width;

		if (world_width % tiles_width != 0) {
			world_width = (world_width / tiles_width) * tiles_width;
		}
		tile_size = world_width / tiles_width;
		if (world_height % tile_size != 0) {
			world_height = (world_height / tile_size) * tile_size;
		}
		tiles_height = world_height / tile_size;

		tile_number = tiles_width * tiles_height;
		camera = cam;
		world_tiles.reserve(tile_number);
		tile_color_buffers.reserve(tile_number);
	}

	bool MouseInBounds(IntVector pos) {
		return ( (pos.x >= 0 && pos.x < GetScreenWidth()) && (pos.y >= 0 && pos.y < GetScreenHeight()) );
	}
	IntVector CursorToWorld(IntVector screen_pos) {
		// Screen mouse position -> Grid scaled mouse position
		// potentially GetScreenToWorld2D ??
		Vector2 the = GetScreenToWorld2D(screen_pos.toVector2(), *camera);
		screen_pos = {(int)the.x, (int)the.y};
		
		if (screen_pos.x < 0) screen_pos.x = 0;
		if (screen_pos.y < 0) screen_pos.y = 0;
		if (screen_pos.x >= world_width ) screen_pos.x = world_width;
		if (screen_pos.y >= world_height ) screen_pos.y = world_height;

		IntVector new_pos = { (int)(screen_pos.x ), (int)(screen_pos.y ) };
		return new_pos;
	}
	IntVector CursorToTile(IntVector grid_pos) {
		// Grid scaled mouse position -> tile position
		grid_pos.x /= tile_size;
		grid_pos.y /= tile_size;

		if (grid_pos.x >= tiles_width) grid_pos.x = tiles_width - 1;
		if (grid_pos.y >= tiles_height) grid_pos.y = tiles_height - 1;

		return grid_pos;
	}
	IntVector CursorToInnerPosition(IntVector grid_pos, IntVector tile_pos) {
		//Grid scaled mouse position + Tile Position -> Mouse position inside tile
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
		for (SandTile* tile : world_tiles) {
			DrawRectangleLines(
				(tile->position.x * tile_size) ,
				(tile->position.y * tile_size) ,
				tile_size , tile_size , RED);
		}
	}

	void DrawEmptyTiles() {
		for (SandTile* tile : world_tiles) {
			if (tile->simulated_cell_count == 0) {
				DrawRectangle(
					(tile->position.x * tile_size) ,
					(tile->position.y * tile_size) ,
					tile_size , tile_size , Color{ 0, 0, 0, 150 });
			}
		}
	}

	void DrawDirtyRecs() {
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
		char fps_msg[8];
		_itoa_s(GetFPS(), fps_msg, 8, 10);
		DrawTextEx(font, fps_msg, {600, 50}, font.baseSize, 0, col);

		char ft_msg[16];
		snprintf(ft_msg, 16, "%.2f ms", GetFrameTime() * 1000);
		DrawTextEx(font, ft_msg, { 600, float(50 + font.baseSize) }, font.baseSize, 0, col);
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

		char mouse_pos[100];
		snprintf(mouse_pos, 100, "X: %d, Y: %d", GetMouseX(), GetMouseY());
		DrawTextEx(font, mouse_pos, { 50, float(50 ) }, font.baseSize, 0, col);

		char mouse_pos_s[100];
		snprintf(mouse_pos_s, 100, "X: %d, Y: %d", scaled_pos.x, scaled_pos.y);
		DrawTextEx(font, mouse_pos_s, { 50, float(50 + font.baseSize * 1) }, font.baseSize, 0, col);

		char tile_num[20];
		snprintf(tile_num, 20, "Tile: %d, %d", tile_pos.x, tile_pos.y);
		DrawTextEx(font, tile_num, { 50, float(50 + font.baseSize * 2 ) }, font.baseSize, 0, col);

		char brush[10];
		snprintf(brush, 10, param_ref[brush_choice].type_name.c_str(), 1);
		DrawTextEx(font, brush, { 50, float(50 + font.baseSize * 3 ) }, font.baseSize, 0, col);

		SandTile* tile = GetTileFromPos(tile_pos);
		char tile_cell_count[10];
		snprintf(tile_cell_count, 10, "%d", tile->simulated_cell_count);
		DrawTextEx(font, tile_cell_count, { 50, float(50 + font.baseSize * 4 ) }, font.baseSize, 0, col);

		DrawCircleLines((int)GetMouseX(), (int)GetMouseY(), (brush_size+1) / 2  * camera->zoom, col);
	}

	void AddMaterialSingleInWorld(IntVector world_pos, ParticleType m_type) {
		if (CursorInWorldBounds(world_pos)) {
			IntVector tile_pos = CursorToTile(world_pos);
			if (m_type == EMPTY) {
				GetTileFromPos(tile_pos)->DeleteMaterialSingle(CursorToInnerPosition(world_pos, tile_pos));
			}
			else {
				GetTileFromPos(tile_pos)->AddMaterialSingle(CursorToInnerPosition(world_pos, tile_pos), m_type);
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
};
