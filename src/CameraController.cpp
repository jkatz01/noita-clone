#pragma once
#include "raylib.h"

class CameraController {
public:
	Camera2D camera;

	CameraController(int screen_width, int screen_height) {
		camera.offset = { screen_width / 2.0f, screen_height / 2.0f };
		camera.target = { screen_width / 2.0f, screen_height / 2.0f };
		camera.rotation = 0.0f;
		camera.zoom = 1.0f;
	}

	void MoveCamera() {
		float ft = GetFrameTime() * 500;
		if (IsKeyDown(KEY_W)) {
			camera.target.y -= 1 * ft;
		}
		if (IsKeyDown(KEY_S)) {
			camera.target.y += 1 * ft;
		}
		if (IsKeyDown(KEY_A)) {
			camera.target.x -= 1 * ft;
		}
		if (IsKeyDown(KEY_D)) {
			camera.target.x += 1 * ft;
		}

		// Camera zoom controls
		camera.zoom += ((float)GetMouseWheelMove() * 0.2f);

		if (camera.zoom > 3.0f) camera.zoom = 3.0f;
		else if (camera.zoom < 0.1f) camera.zoom = 0.1f;
	}
	
};