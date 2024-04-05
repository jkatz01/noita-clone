#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#define LIGHTBLUE ColorFromHSV(200, 0.78, 0.92)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

enum ParticleType {
	DIRT,
	SAND,
	AIR,
	WATER,
	EMPTY
};

typedef struct {
	enum ParticleType type;
	int updated; // Updated this frame
} Particle;

void generate_world(Particle **world, int world_size) {
	for (int i = 0; i < world_size; i++) {
		for (int j = 0; j < world_size; j++) {
			world[i][j].updated = 0;
			if (i > 40 && i < 60 && j > 40 && j < 60) {
				world[i][j].type = SAND;	
			}
			else {
				world[i][j].type = EMPTY;	
			}
		}
	}
}

void add_material(Particle **world, int x, int y, int world_size, int brush_size, enum ParticleType mt_type) {
	if (y < 0 || y >= world_size) {
		return;
	}
	if (x < 0 || x >= world_size) {
		return;
	}
	for (int i = y; i < MIN(world_size, y + brush_size); i++) {
		for (int j = x; j < MIN(world_size, x + brush_size); j++) { //TODO: handle this better and without a macro
			world[i][j].type = mt_type;
		}
	}
}

void delete_material(Particle **world, int x, int y, int world_size, int brush_size) {
	if (y < 0 || y >= world_size) {
		return;
	}
	if (x < 0 || x >= world_size) {
		return;
	}
	for (int i = y; i < MIN(world_size, y + brush_size); i++) {
		for (int j = x; j < MIN(world_size, x + brush_size); j++) {
			world[i][j].type = EMPTY;
		}
	}
}

Color color_lookup(enum ParticleType type) {
	switch (type) {
		case AIR: return LIGHTBLUE;
		case DIRT: return DARKBROWN;
		case SAND: return BROWN;
		case WATER: return BLUE;
		case EMPTY: return BLACK;
		default: return RED;
	}
}

int idx_down_exists(int x, int y, int world_size) {
	if (y < world_size - 1) return 1;
	else return 0;
}
int idx_left_exists(int x, int y, int world_size) {
	if (x > 1) return 1;
	else return 0;
}
int idx_down_left_exists(int x, int y, int world_size) {
	if (y < world_size - 1 && x > 1) return 1;
	else return 0;
}
int idx_right_exists(int x, int y, int world_size) {
	if (x < world_size-1) return 1;
	else return 0;
}
int idx_down_right_exists(int x, int y, int world_size) {
	if (y < world_size - 1 && x < world_size - 1) return 1;
	else return 0;
}

void update_sand(Particle **world, int x, int y, int world_size) {
	if (idx_down_exists(x, y, world_size) && world[y+1][x].type == EMPTY) {
		world[y+1][x].type = SAND;
		world[y+1][x].updated = 1;
		world[y][x].type = EMPTY;
		return;
	}
	else if (idx_down_left_exists(x, y, world_size) && world[y+1][x-1].type == EMPTY) {
		world[y+1][x-1].type = SAND;
		world[y+1][x-1].updated = 1;
		world[y][x].type = EMPTY;
		return;
	}
	else if (idx_down_right_exists(x, y, world_size) && world[y+1][x+1].type == EMPTY) {
		world[y+1][x+1].type = SAND;
		world[y+1][x+1].updated = 1;
		world[y][x].type = EMPTY;
		return;
	} 
}

void update_water(Particle **world, int x, int y, int world_size) {
	if (idx_down_exists(x, y, world_size) && world[y+1][x].type == EMPTY) {
		world[y+1][x].type = WATER;
		world[y+1][x].updated = 1;
		world[y][x].type = EMPTY;
		return;
	}
	else if (idx_down_left_exists(x, y, world_size) && world[y+1][x-1].type == EMPTY) {
		world[y+1][x-1].type = WATER;
		world[y+1][x-1].updated = 1;
		world[y][x].type = EMPTY;
		return;
	}
	else if (idx_down_right_exists(x, y, world_size) && world[y+1][x+1].type == EMPTY) {
		world[y+1][x+1].type = WATER;
		world[y+1][x+1].updated = 1;
		world[y][x].type = EMPTY;
		return;
	} 
	else if (idx_left_exists(x, y, world_size) && world[y][x-1].type == EMPTY) {
		world[y][x-1].type = WATER;
		world[y][x-1].updated = 1;
		world[y][x].type = EMPTY;
		return;
	} 
	else if (idx_right_exists(x, y, world_size) && world[y][x+1].type == EMPTY) {
		world[y][x+1].type = WATER;
		world[y][x+1].updated = 1;
		world[y][x].type = EMPTY;
		return;
	} 
}

void update_me(Particle **world, int x, int y, int world_size) {
	enum ParticleType my_type = world[y][x].type;
	if (world[y][x].updated) {
		world[y][x].updated = 0;
		return ;
	}
	switch (my_type) {
		case EMPTY: break;
		case DIRT: break;
		case AIR: break;
		case SAND: update_sand(world, x, y, world_size); break;
		case WATER: update_water(world, x, y, world_size); break;
	}
}

int main() {
	int world_size = 200;
	int screen_size = 800;
	int scaled_size = screen_size / world_size;

	InitWindow(screen_size, screen_size, "World");
	SetTargetFPS(300);
	
	Particle *_world = (Particle*)malloc(world_size * world_size * sizeof(Particle));
	if (!_world) return -1;
	Particle **world = (Particle**)malloc(world_size * sizeof(Particle*));
	if (!world) {
		free(_world);
		return -1;
	}
	for (int i = 0; i < world_size; i++) {
		world[i] = &(_world[i*world_size]);
	}

	// World Generation
	generate_world(world, world_size);
	
	int mouse_x, mouse_y;
	
	while (!WindowShouldClose()) {
		mouse_x = (int)GetMouseX() / scaled_size;
		mouse_y = (int)GetMouseY() / scaled_size;
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) add_material(world, mouse_x, mouse_y, world_size, 10, SAND);
		if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) add_material(world, mouse_x, mouse_y, world_size, 10, WATER);
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) delete_material(world, mouse_x, mouse_y, world_size, 10);

		//TODO: add world context struct

		BeginDrawing();
		ClearBackground(DARKGRAY);
		for (int i = 0; i < world_size; i++) {
			for (int j = 0; j < world_size; j++) {
				update_me(world, j, i, world_size);
			}
		}
		for (int i = 0; i < world_size; i++) {
			for (int j = 0; j < world_size; j++) {
				DrawRectangle(j*scaled_size, i*scaled_size, scaled_size, scaled_size, color_lookup(world[i][j].type));
			}
		}
		EndDrawing();
	}
	CloseWindow();
	free(world);
	return 0;
}
