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
			if (i > 200 && i < 220 && j > 200 && j < 220) {

				world[i][j].type = SAND;	
			}
			else {
				world[i][j].type = EMPTY;	
			}
		}
	}
}

Color color_lookup(enum ParticleType type) {
	if (type == AIR) return LIGHTBLUE;
	else if (type == DIRT) return DARKBROWN;
	else if (type == SAND) return BROWN;
	else if (type == EMPTY) return BLACK;
	else return RED;
}

void update_sand(Particle **world, int x, int y, int world_size) {
	if (y < world_size-1 && world[y+1][x].type == EMPTY) {
		world[y+1][x].type = SAND;
		world[y][x].type = EMPTY;
		return;
	}
	else if (y < world_size-1 && x > 1 && world[y+1][x-1].type == EMPTY) {
		world[y+1][x-1].type = SAND;
		world[y][x].type = EMPTY;
		return;
	}
	else if (y < world_size-1 && x < world_size - 1 && world[y+1][x+1].type == EMPTY) {
		world[y+1][x+1].type = SAND;
		world[y][x].type = EMPTY;
		return;
	} 
}
void update_me(Particle **world, int x, int y, int world_size) {
	enum ParticleType my_type = world[y][x].type;
	if (my_type == SAND) {
		update_sand(world, x, y, world_size);
	}
}

int main() {
	int world_size = 800;
	InitWindow(world_size, world_size, "World");
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
		for (int i = 0; i < world_size; i++) {
			for (int j = 0; j < world_size; j++) {
				DrawPixel(j, i, color_lookup(world[i][j].type));
			}
		}
		EndDrawing();
	}
	CloseWindow();
	free(world);
	return 0;
}
