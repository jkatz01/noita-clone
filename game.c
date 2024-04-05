#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#define LIGHTBLUE ColorFromHSV(200, 0.78, 0.92)

enum ParticleType {
	DIRT,
	SAND,
	AIR,
	EMPTY
};

typedef struct {
	enum ParticleType type;
} Particle;

void generate_world(Particle **world, int world_size) {
	for (int i = 0; i < world_size; i++) {
		for (int j = 0; j < world_size; j++) {
			if (i > 20 && i < 30 && j > 20 && j < 30) {

				world[i][j].type = SAND;	
			}
			else {
				world[i][j].type = EMPTY;	
			}
		}
	}
}

Color color_lookup(enum ParticleType type) {
	switch (type) {
		case AIR: return LIGHTBLUE;
		case DIRT: return DARKBROWN;
		case SAND: return BROWN;
		case EMPTY: return BLACK;
		default: return RED;
	}
}

int idx_down_exists(int x, int y, int world_size) {
	if (y < world_size - 1) return 1;
	else return 0;
}
int idx_down_left_exists(int x, int y, int world_size) {
	if (y < world_size - 1 && x > 1) return 1;
	else return 0;
}
int idx_down_right_exists(int x, int y, int world_size) {
	if (y < world_size - 1 && x < world_size - 1) return 1;
	else return 0;
}

void update_sand(Particle **world, int x, int y, int world_size) {
	if (idx_down_exists(x, y, world_size) && world[y+1][x].type == EMPTY) {
		world[y+1][x].type = SAND;
		world[y][x].type = EMPTY;
		return;
	}
	else if (idx_down_left_exists(x, y, world_size) && world[y+1][x-1].type == EMPTY) {
		world[y+1][x-1].type = SAND;
		world[y][x].type = EMPTY;
		return;
	}
	else if (idx_down_right_exists(x, y, world_size) && world[y+1][x+1].type == EMPTY) {
		world[y+1][x+1].type = SAND;
		world[y][x].type = EMPTY;
		return;
	} 
}
void update_me(Particle **world, int x, int y, int world_size) {
	enum ParticleType my_type = world[y][x].type;
	switch (my_type) {
		case EMPTY: break;
		case DIRT: break;
		case AIR: break;
		case SAND: update_sand(world, x, y, world_size); break;
	}
}

int main() {
	int world_size = 100;
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

	
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);
		for (int i = world_size-1; i > 0; --i) {
			for (int j = 0; j < world_size; j++) {
				update_me(world, j, i, world_size);
			}
		}
		// TODO: render everything at once
		for (int i = 0; i < world_size; i++) {
			for (int j = 0; j < world_size; j++) {
				DrawRectangle(j*scaled_size, i*scaled_size, scaled_size, scaled_size, color_lookup(world[i][j].type));
			}
		}
		if (IsKeyDown(KEY_ENTER)) generate_world(world, world_size);
		EndDrawing();
	}
	CloseWindow();
	free(world);
	return 0;
}
