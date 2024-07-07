/*
 * by: dominus
 * heheuhh
 */


#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#define LIGHTBLUE ColorFromHSV(200, 0.78, 0.92)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

const int world_size = 200;
const int screen_size = 800;
const int scaled_size = screen_size / world_size;

enum ParticleType {
	STONE,
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

int three_min(int x, int y, int z) {
	int val = MIN(x, y);
	val = MIN(val, z);
	return val;
}

void add_material(Particle **world, int x, int y, int world_size, int brush_size, enum ParticleType mt_type) {
	static int previous_x, previous_y;

	if (y < 0 || y >= world_size) {
		return;
	}
	if (x < 0 || x >= world_size) {
		return;
	}
	BeginDrawing();
	DrawRectangleLines(x*scaled_size - 10, y*scaled_size - 10, 20, 20, GREEN);
	DrawRectangleLines(previous_x*scaled_size - 10, previous_y*scaled_size - 10, 20, 20, BLUE);
	Vector2 startPos = {x*scaled_size, y*scaled_size};
	Vector2 endPos = {previous_x*scaled_size, previous_y*scaled_size};

	for (int i = 0; i < world_size; i++) {
		for (int j = 0; j < world_size; j++) {
			DrawRectangleLines(MIN(previous_x, x)*scaled_size, MIN(previous_y, y)*scaled_size, (MAX(previous_x, x) - MIN(previous_x, x))*scaled_size, (MAX(previous_y, y) - MIN(previous_y, y))*scaled_size, GREEN);
			if (CheckCollisionPointLine((Vector2){ j*scaled_size, i*scaled_size } ,startPos, endPos, 20)) {
				world[i][j].type = mt_type;
			}
			
		}
	}
	EndDrawing();

	previous_x = x;
	previous_y = y;
	
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
		case STONE: return DARKBROWN;
		case SAND: return BROWN;
		case WATER: return BLUE;
		case EMPTY: return BLACK;
		default: return RED;
	}
}

int density_lookup(enum ParticleType type) {
	switch (type) {
		case AIR: return 1;
		case STONE: return 5;
		case SAND: return 5;
		case WATER: return 2;
		case EMPTY: return 0;
		default: return 0;
	}
}

int idx_down_exists(int x, int y, int world_size) {
	if (y < world_size - 1) return 1;
	else return 0;
}
int idx_left_exists(int x, int y, int world_size) {
	if (x > 0) return 1;
	else return 0;
}
int idx_down_left_exists(int x, int y, int world_size) {
	if (y < world_size - 1 && x > 0) return 1;
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

void move_down(Particle **world, int x, int y) {
	enum ParticleType r_type = world[y+1][x].type;
	world[y+1][x].type = world[y][x].type;
	world[y+1][x].updated = 1;
	world[y][x].type = r_type;
}
void move_down_right(Particle **world, int x, int y) {
	enum ParticleType r_type = world[y+1][x+1].type;
	world[y+1][x+1].type = world[y][x].type;
	world[y+1][x+1].updated = 1;
	world[y][x].type = r_type;
}
void move_down_left(Particle **world, int x, int y) {
	enum ParticleType r_type = world[y+1][x-1].type;
	world[y+1][x-1].type = world[y][x].type;
	world[y+1][x-1].updated = 1;
	world[y][x].type = r_type;
}
void move_right(Particle **world, int x, int y) {
	enum ParticleType r_type = world[y][x+1].type;
	world[y][x+1].type = world[y][x].type;
	world[y][x+1].updated = 1;
	world[y][x].type = r_type;
}
void move_left(Particle **world, int x, int y) {
	enum ParticleType r_type = world[y][x-1].type;
	world[y][x-1].type = world[y][x].type;
	world[y][x-1].updated = 1;
	world[y][x].type = r_type;
}
void update_sand(Particle **world, int x, int y, int world_size) {
	if (idx_down_exists(x, y, world_size) && density_lookup(world[y+1][x].type) < density_lookup(world[y][x].type)) {
		move_down(world, x, y);
		return;
	}
	else if (idx_down_left_exists(x, y, world_size) &&density_lookup(world[y+1][x-1].type) < density_lookup(world[y][x].type)) {
		move_down_left(world,x, y);
		return;
	}
	else if (idx_down_right_exists(x, y, world_size) &&density_lookup(world[y+1][x+1].type) < density_lookup(world[y][x].type)) {
		move_down_right(world, x, y);
		return;
	} 
}
void update_water(Particle **world, int x, int y, int world_size) {
	if (idx_down_exists(x, y, world_size) && world[y+1][x].type == EMPTY) {
		move_down(world, x, y);
		return;
	}
	else if (idx_down_left_exists(x, y, world_size) && world[y+1][x-1].type == EMPTY) {
		move_down_left(world,x, y);
		return;
	}
	else if (idx_down_right_exists(x, y, world_size) && world[y+1][x+1].type == EMPTY) {
		move_down_right(world, x, y);
		return;
	} 
	if (idx_left_exists(x, y, world_size) && world[y][x-1].type == EMPTY) {
		move_left(world, x, y);
		/*if (idx_left_exists(x-1, y, world_size) && world[y][x-2].type != EMPTY) {
			;	
		}
		else {
			move_left(world, x, y);
			return;
		}*/
	}
	if (idx_right_exists(x, y, world_size) && world[y][x+1].type == EMPTY) {
		move_right(world, x, y);
		/*if (idx_right_exists(x+1, y, world_size) && world[y][x+2].type != EMPTY) {
			;
		}
		else {
			move_right(world, x, y);
			return;
		}*/
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
		case STONE: break;
		case AIR: break;
		case SAND: update_sand(world, x, y, world_size); break;
		case WATER: update_water(world, x, y, world_size); break;
	}
}

int main() {

	InitWindow(screen_size, screen_size, "World");
	SetTargetFPS(0);
	
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

	generate_world(world, world_size);
	
	int mouse_x, mouse_y;
	enum ParticleType materials[3] = {SAND, WATER, STONE};
	enum ParticleType m_choice = SAND;

	while (!WindowShouldClose()) {
		mouse_x = (int)GetMouseX() / scaled_size;
		mouse_y = (int)GetMouseY() / scaled_size;
		if (IsKeyPressed(KEY_ONE)) m_choice = materials[0];
		else if (IsKeyPressed(KEY_TWO)) {m_choice = materials[1]; printf("Key two pressed\n");}
		else if (IsKeyPressed(KEY_THREE)) m_choice = materials[2];
		else if (IsKeyPressed(KEY_ENTER)) generate_world(world, world_size);
		
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) add_material(world, mouse_x, mouse_y, world_size, 5, m_choice);
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) delete_material(world, mouse_x, mouse_y, world_size, 5);

		//TODO: add world context struct

		BeginDrawing();
		ClearBackground(DARKGRAY);
		for (int i = world_size-1; i > 0 ; --i) {
			for (int j = 0; j < world_size; j++) {
				update_me(world, j, i, world_size);
			}
		}
		for (int i = 0; i < world_size; i++) {
			for (int j = 0; j < world_size; j++) {
				DrawRectangle(j*scaled_size, i*scaled_size, scaled_size, scaled_size, color_lookup(world[i][j].type));
			}
		}

		DrawText("1 - Sand | 2 - Water | 3 - Stone | RMB - Delete", 100, 40, 24, WHITE);
		EndDrawing();
	}
	CloseWindow();
	free(world);
	return 0;
}
