#include <stdlib.h>
#include "raylib.h"

#define LIGHTBLUE ColorFromHSV(200, 0.78, 0.92)

enum ParticleType {
	DIRT,
	AIR,
	EMPTY
};

typedef struct {
	enum ParticleType type;
	enum ParticleType next_type;
	int updated;
} Particle;

void generate_world(Particle **world, int world_size) {
	for (int i = 0; i < world_size/2; i++) {
		for (int j = 0; j < world_size; j++) {
			if (j % 3 == 0) {
				world[i][j].type = AIR;
			}
			else {
				world[i][j].type = DIRT;	
			}
			world[i][j].updated = 0;	
		}
	}
	for (int i = world_size/2; i < world_size; i++) {
		for (int j = 0; j < world_size; j++) {
			world[i][j].type = DIRT;	
			world[i][j].updated = 0;	
		}
	}
}

Color color_lookup(enum ParticleType type) {
	if (type == AIR) return LIGHTBLUE;
	if (type == DIRT) return DARKBROWN;
	else return RED;
}

void update_me(Particle **world, int x, int y, int world_size) {
	enum ParticleType my_type = world[y][x].type;
	enum ParticleType below = EMPTY;
	if (y < world_size - 1) {
		below = world[y+1][x].type;
	}
	else {
		return;
	}
	if (my_type == AIR && below == DIRT) {
		world[y+1][x].next_type = AIR;
		world[y+1][x].updated = 1;
	}
	DrawPixel(x, y, color_lookup(world[y][x].type));
	if (world[y][x].updated) {
		world[y][x].type = world[y][x].next_type;
		world[y][x].updated = 0;
	}
}

int main() {
	int world_size = 800;
	InitWindow(world_size, world_size, "World");
	SetTargetFPS(10);
	
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
		for (int i = 0; i < world_size; i++) {
			for (int j = 0; j < world_size; j++) {
				update_me(world, j, i, world_size);
			}
		}
		EndDrawing();
	}
	CloseWindow();
	free(world);
	return 0;
}
