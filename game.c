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

const int gravity = 2;

enum ParticleType {
	STONE,
	SAND,
	AIR,
	WATER,
	LAVA,
	EMPTY
};

typedef struct  {
	int up_left, left, down_left, down, down_right, right, up_right, up;
} PixelSurroundings;

typedef struct {
	enum ParticleType type;
	int updated; // Updated this frame
	Color color;
	Vector2 velocity;
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
		case LAVA: return ColorTint(ColorBrightness(RED, rand_range(0, 0.3)), ORANGE);
		case EMPTY: return CLITERAL(Color){ 0, 0, 0, 20 } ;
		default: return RED;
	}
}

int density_lookup(enum ParticleType type) {
	switch (type) {
		case AIR: return 0;
		case STONE: return 3;
		case SAND: return 3;
		case WATER: return 1;
		case LAVA: return 2;
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
//up_left, , , , , , up_right, up;
PixelSurroundings check_bounds(int x, int y, int world_size) {
	PixelSurroundings me = {0, 0, 0, 0, 0, 0, 0, 0};
	if (y < world_size - 1) me.down = 1;
	if (x > 0) me.left = 1;
	if (y < world_size - 1 && x > 0) me.down_left = 1;
	if (x < world_size-1) me.right = 1;
	if (y < world_size - 1 && x < world_size - 1) me.down_right = 1;
	return me;
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
	//world[y][x+1].updated = 1;
	world[y][x] = r_part;
}
void move_left(Particle **world, int x, int y) {
	
	Particle r_part = world[y][x-1];
	world[y][x-1] = world[y][x];
	//world[y][x-1].updated = 1;
	world[y][x] = r_part;
}

void move_by_velocity(Particle **world, int x, int y, Vector2 vel) {
	int cur_x = x;
	int cur_y = y;
	
	if (vel.y > 1 && cur_y < world_size-2 && density_lookup(world[cur_y][cur_x].type) - density_lookup(world[cur_y-1][cur_x].type) == 1) {
		if (x % 2 == 0) vel.x += 5;
		else 			vel.x -= 5;
		vel.y -= 1;
	}
	
	int desired_x = x + (int)vel.x;
	int desired_y = y + (int)vel.y;
	
	
	if (desired_x > world_size) desired_x=world_size;
	if (desired_x < 0) 			desired_x = 0;
	if (desired_y > world_size-1) desired_y=world_size-1;
	if (desired_y < 0) 			desired_y = 0;
	
	
	
	// X direction
	if (vel.x < 0) {
		while (cur_x != desired_x) {
			if (density_lookup(world[cur_y][cur_x-1].type) < density_lookup(world[cur_y][cur_x].type)) {
				move_left(world, cur_x, cur_y);
				cur_x--;
			}
			else break;
		}
	}
	else if (vel.x > 0) {
		while (cur_x != desired_x) {
			if (density_lookup(world[cur_y][cur_x+1].type) < density_lookup(world[cur_y][cur_x].type)) {
				move_right(world, cur_x, cur_y);
				cur_x++;
			}
			else break;
		}
	}

	// Y direction
	if (vel.y < 0) {
		;
	}
	else if (vel.y > 0) {
		while (cur_y != desired_y) {
			if (density_lookup(world[cur_y+1][cur_x].type) < density_lookup(world[cur_y][cur_x].type)) {
				move_down(world, cur_x, cur_y);
				cur_y++;
			}
			else break;
		}
	}
	world[cur_y][cur_x].updated = 1;
}

void update_powder(Particle **world, int x, int y, int world_size) {
	PixelSurroundings my_pos = check_bounds(x, y, world_size);
	if (my_pos.down && density_lookup(world[y+1][x].type) < density_lookup(world[y][x].type)) {
		if (world[y+1][x].updated == 1) {
			return;
		}
		Vector2 test_vel = {0, 3};
		move_by_velocity(world, x, y, test_vel);
		return;
	}
	else if (my_pos.down_left &&density_lookup(world[y+1][x-1].type) < density_lookup(world[y][x].type)) {
		if (world[y+1][x-1].updated == 1) {
			return;
		}
		Vector2 test_vel = {-2, 3};
		move_by_velocity(world, x, y, test_vel);
		return;
	}
	else if (my_pos.down_right &&density_lookup(world[y+1][x+1].type) < density_lookup(world[y][x].type)) {
		if (world[y+1][x+1].updated == 1) {
			return;
		}
		Vector2 test_vel = {2, 3};
		move_by_velocity(world, x, y, test_vel);
		return;
	} 
}
void update_liquid(Particle **world, int x, int y, int world_size) {
	PixelSurroundings my_pos = check_bounds(x, y, world_size);
	//int odd = x % 2;
	//if odd()
	if (my_pos.down &&density_lookup(world[y+1][x].type) < density_lookup(world[y][x].type)) {
		Vector2 test_vel = {0, 3};
		move_by_velocity(world, x, y, test_vel);
		return;
	}
	
	if (my_pos.down_right &&density_lookup(world[y+1][x+1].type) < density_lookup(world[y][x].type)) {
		Vector2 test_vel = {4, 3};
		move_by_velocity(world, x, y, test_vel);
		return;
	} 
	if (my_pos.down_left &&density_lookup(world[y+1][x-1].type) < density_lookup(world[y][x].type)) {
		Vector2 test_vel = {-4, 3};
		move_by_velocity(world, x, y, test_vel);
		return;
	}
	if (my_pos.right &&density_lookup(world[y][x+1].type) < density_lookup(world[y][x].type)) {
		Vector2 test_vel = {4, 0};
		move_by_velocity(world, x, y, test_vel);
		return;
	}
	if (my_pos.left &&density_lookup(world[y][x-1].type) < density_lookup(world[y][x].type)) {
		Vector2 test_vel = {-4, 0};
		move_by_velocity(world, x, y, test_vel);
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
		case STONE: break;
		case AIR: break;
		case SAND: update_powder(world, x, y, world_size); break;
		case WATER: update_liquid(world, x, y, world_size); break;
		case LAVA: update_liquid(world, x, y, world_size); break;
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
	enum ParticleType materials[4] = {SAND, WATER, STONE, LAVA};
	enum ParticleType m_choice = SAND;
	
	int l_mouse_held_last_frame = 0;
	int r_mouse_held_last_frame = 0;
	
	Image buddyworld = LoadImage("assets/beautifu.png");
	//ImageCrop(&buddyworld, (Rectangle){ 100, 10, 280, 380 });      // Crop an image piece
    //ImageFlipHorizontal(&buddyworld);                              // Flip cropped image horizontally
    ImageResize(&buddyworld, screen_size, screen_size+100);                            // Resize flipped-cropped image
	Texture2D bg_texture = LoadTextureFromImage(buddyworld);
	UnloadImage(buddyworld);
	
	int brush_size = 10;
	int simulate = 1;

	while (!WindowShouldClose()) {
		mouse_x = (int)GetMouseX() / scaled_size;
		mouse_y = (int)GetMouseY() / scaled_size;
		
		if (IsKeyPressed(KEY_ENTER)) simulate = (1 - simulate);
		if (IsKeyPressed(KEY_ONE)) m_choice = materials[0];
		else if (IsKeyPressed(KEY_TWO)) {m_choice = materials[1];}
		else if (IsKeyPressed(KEY_THREE)) m_choice = materials[2];
		else if (IsKeyPressed(KEY_FOUR)) m_choice = materials[3];
		else if (IsKeyPressed(KEY_J)) generate_world(world, world_size);
		else if (IsKeyPressed(KEY_B)) brush_size += 5;
		else if (IsKeyPressed(KEY_V)) brush_size -= 5;

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			add_material(world, mouse_x, mouse_y, world_size, brush_size, m_choice, l_mouse_held_last_frame);
			l_mouse_held_last_frame = 1;
		}
		else {
			l_mouse_held_last_frame = 0;
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			delete_material(world, mouse_x, mouse_y, world_size, brush_size, r_mouse_held_last_frame);
			r_mouse_held_last_frame = 1;
		}
		else {
			r_mouse_held_last_frame = 0;
		}
		
		//TODO: change the collision to work based on speed
		

		BeginDrawing();
		ClearBackground(DARKGRAY);
		
		DrawTexture(bg_texture, screen_size/2 - bg_texture.width/2, screen_size/2 - bg_texture.height/2 - 40, WHITE);
		
		if (simulate)
		{
			for (int i = world_size-1; i > 0 ; --i) {
				if (i % 2 == 0) {
					for (int j = 0; j < world_size; j++) {
						update_me(world, j, i, world_size);
					}
				}
				else {
					for (int j = world_size-1; j > 0; j--) {
						update_me(world, j, i, world_size);
					}
				}
				
			}
		}
		for (int i = 0; i < world_size; i++) {
			for (int j = 0; j < world_size; j++) {
				DrawRectangle(j*scaled_size, i*scaled_size, scaled_size, scaled_size, world[i][j].color);
			}
		}

		DrawText("1 - Sand | 2 - Water | 3 - Stone | 4 - Lava | RMB - Delete \n\nB/V - Brush | ENTER - Toggle | J - Reset", 100, 40, 24, WHITE);
		char fps_msg[8];
		_itoa_s(GetFPS(), fps_msg, 8, 10);
		DrawText(fps_msg, 600, 160, 40, BLACK);
		EndDrawing();
	}
	CloseWindow();
	free(world);
	return 0;
}
