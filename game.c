/*
 * by: dominus
 * heheuhh
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"

#define LIGHTBLUE ColorFromHSV(200, 0.78, 0.92)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define WATER_BLUE       CLITERAL(Color){ 0, 121, 241, 150 } 

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
	Color color;
} Particle;

float rand_range(float min, float max)
{
	float random = (float)rand() / RAND_MAX;
	float range = max - min;
	return (random * range) + min;
}

Color color_lookup(enum ParticleType type) {
	switch (type) {
		case AIR: return LIGHTBLUE;
		case STONE: return DARKBROWN;
		case SAND: return ColorBrightness(BROWN, rand_range(-0.3, 0.3));
		case WATER: return ColorBrightness(WATER_BLUE, rand_range(-0.1, 0.1));
		case EMPTY: return CLITERAL(Color){ 0, 0, 0, 20 } ;
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

void generate_world(Particle **world, int world_size) {
	for (int i = 0; i < world_size; i++) {
		for (int j = 0; j < world_size; j++) {
			world[i][j].updated = 0;
			if (i > 40 && i < 60 && j > 40 && j < 60) {
				world[i][j].type = SAND;
				world[i][j].color = color_lookup(SAND);
			}
			else {
				world[i][j].type = EMPTY;	
				world[i][j].color = color_lookup(EMPTY);
			}
		}
	}
}

int three_min(int x, int y, int z) {
	int val = MIN(x, y);
	val = MIN(val, z);
	return val;
}

void add_material(Particle **world, int x, int y, int world_size, int brush_size, enum ParticleType mt_type, int mouse_held) {
	static int previous_x, previous_y;
	if (mouse_held == 0) {
		previous_x = x;
		previous_y = y;
	}
	if (y < 0 || y >= world_size) {
		return;
	}
	if (x < 0 || x >= world_size) {
		return;
	}
	
	if (previous_x == x  && previous_y == y) {
		x += brush_size/scaled_size;
		y += brush_size/scaled_size;
	}
	
	Vector2 startPos = {x*scaled_size, y*scaled_size};
	Vector2 endPos = {previous_x*scaled_size, previous_y*scaled_size};
	


	for (int i = 0; i < world_size; i++) {
		for (int j = 0; j < world_size; j++) {
			DrawRectangleLines(MIN(previous_x, x)*scaled_size, MIN(previous_y, y)*scaled_size, (MAX(previous_x, x) - MIN(previous_x, x))*scaled_size, (MAX(previous_y, y) - MIN(previous_y, y))*scaled_size, GREEN);
			if (CheckCollisionPointLine((Vector2){ j*scaled_size, i*scaled_size } ,startPos, endPos, brush_size)) {
				if(world[i][j].type == EMPTY) {
					world[i][j].type = mt_type;
					world[i][j].color = color_lookup(mt_type);
				}
			}
			
		}
	}

	previous_x = x;
	previous_y = y;
	
}


void delete_material(Particle **world, int x, int y, int world_size, int brush_size, int mouse_held) {
	static int previous_x, previous_y;
	if (mouse_held == 0) {
		previous_x = x;
		previous_y = y;
	}
	if (y < 0 || y >= world_size) {
		return;
	}
	if (x < 0 || x >= world_size) {
		return;
	}
	if (previous_x == x  && previous_y == y) {
		x += brush_size/scaled_size;
		y += brush_size/scaled_size;
	}
	
	Vector2 startPos = {x*scaled_size, y*scaled_size};
	Vector2 endPos = {previous_x*scaled_size, previous_y*scaled_size};

	for (int i = 0; i < world_size; i++) {
		for (int j = 0; j < world_size; j++) {
			DrawRectangleLines(MIN(previous_x, x)*scaled_size, MIN(previous_y, y)*scaled_size, (MAX(previous_x, x) - MIN(previous_x, x))*scaled_size, (MAX(previous_y, y) - MIN(previous_y, y))*scaled_size, GREEN);
			if (CheckCollisionPointLine((Vector2){ j*scaled_size, i*scaled_size } ,startPos, endPos, brush_size)) {
				world[i][j].type = EMPTY;
				world[i][j].color = color_lookup(EMPTY);
			}
			
		}
	}

	previous_x = x;
	previous_y = y;
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

	Particle r_part = world[y+1][x];
	world[y+1][x] = world[y][x];
	world[y+1][x].updated = 1;
	world[y][x] = r_part;
}
void move_down_right(Particle **world, int x, int y) {
	Particle r_part = world[y+1][x+1];
	world[y+1][x+1] = world[y][x];
	world[y+1][x+1].updated = 1;
	world[y][x] = r_part;
}
void move_down_left(Particle **world, int x, int y) {
	Particle r_part = world[y+1][x-1];
	world[y+1][x-1] = world[y][x];
	world[y+1][x-1].updated = 1;
	world[y][x] = r_part;
}
void move_right(Particle **world, int x, int y) {
	Particle r_part = world[y][x+1];
	world[y][x+1] = world[y][x];
	world[y][x+1].updated = 1;
	world[y][x] = r_part;
}
void move_left(Particle **world, int x, int y) {
	
	Particle r_part = world[y][x-1];
	world[y][x-1] = world[y][x];
	world[y][x-1].updated = 1;
	world[y][x] = r_part;
}
void update_powder(Particle **world, int x, int y, int world_size) {
	if (idx_down_exists(x, y, world_size) && density_lookup(world[y+1][x].type) < density_lookup(world[y][x].type)) {
		if (world[y+1][x].updated == 1) {
			return;
		}
		move_down(world, x, y);
		return;
	}
	else if (idx_down_left_exists(x, y, world_size) &&density_lookup(world[y+1][x-1].type) < density_lookup(world[y][x].type)) {
		if (world[y+1][x-1].updated == 1) {
			return;
		}
		move_down_left(world,x, y);
		return;
	}
	else if (idx_down_right_exists(x, y, world_size) &&density_lookup(world[y+1][x+1].type) < density_lookup(world[y][x].type)) {
		if (world[y+1][x+1].updated == 1) {
			return;
		}
		move_down_right(world, x, y);
		return;
	} 
}
void update_liquid(Particle **world, int x, int y, int world_size) {
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
	}
	if (idx_right_exists(x, y, world_size) && world[y][x+1].type == EMPTY) {
		move_right(world, x, y);
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
		case SAND: update_powder(world, x, y, world_size); break;
		case WATER: update_liquid(world, x, y, world_size); break;
	}
}

int main() {

	InitWindow(screen_size, screen_size, "World");
	SetTargetFPS(0);
	
	srand(time(NULL));
	
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
	
	int l_mouse_held_last_frame = 0;
	int r_mouse_held_last_frame = 0;
	
	Image buddyworld = LoadImage("assets/buddyworld.png");
	//ImageCrop(&buddyworld, (Rectangle){ 100, 10, 280, 380 });      // Crop an image piece
    //ImageFlipHorizontal(&buddyworld);                              // Flip cropped image horizontally
    ImageResize(&buddyworld, 800, 900);                            // Resize flipped-cropped image
	Texture2D bg_texture = LoadTextureFromImage(buddyworld);
	UnloadImage(buddyworld);

	while (!WindowShouldClose()) {
		mouse_x = (int)GetMouseX() / scaled_size;
		mouse_y = (int)GetMouseY() / scaled_size;
		if (IsKeyPressed(KEY_ONE)) m_choice = materials[0];
		else if (IsKeyPressed(KEY_TWO)) {m_choice = materials[1]; printf("Key two pressed\n");}
		else if (IsKeyPressed(KEY_THREE)) m_choice = materials[2];
		else if (IsKeyPressed(KEY_ENTER)) generate_world(world, world_size);

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			add_material(world, mouse_x, mouse_y, world_size, 15, m_choice, l_mouse_held_last_frame);
			l_mouse_held_last_frame = 1;
		}
		else {
			l_mouse_held_last_frame = 0;
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			delete_material(world, mouse_x, mouse_y, world_size, 15, r_mouse_held_last_frame);
			r_mouse_held_last_frame = 1;
		}
		else {
			r_mouse_held_last_frame = 0;
		}
		
		//TODO: change the collision to work based on speed
		

		BeginDrawing();
		ClearBackground(DARKGRAY);
		
		DrawTexture(bg_texture, screen_size/2 - bg_texture.width/2, screen_size/2 - bg_texture.height/2 - 40, WHITE);
		
		for (int i = world_size-1; i > 0 ; --i) {
			for (int j = 0; j < world_size; j++) {
				update_me(world, j, i, world_size);
			}
		}
		for (int i = 0; i < world_size; i++) {
			for (int j = 0; j < world_size; j++) {
				DrawRectangle(j*scaled_size, i*scaled_size, scaled_size, scaled_size, world[i][j].color);
			}
		}

		DrawText("1 - Sand | 2 - Water | 3 - Stone | RMB - Delete", 100, 40, 24, WHITE);
		EndDrawing();
	}
	CloseWindow();
	free(world);
	return 0;
}
