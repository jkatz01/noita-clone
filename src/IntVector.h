#include <cmath>
#include "raylib.h"

#pragma once
class IntVector {
public:

	int x, y;

	IntVector() {
		x = -9999;
		y = -9999;
	}

	IntVector(int _x, int _y) {
		x = _x;
		y = _y;
	}

	// TODO: write compare function

	bool operator==(const IntVector& rhs) const {
		return (x == rhs.x && y == rhs.y);
	}

	double dist(IntVector vec2) {
		int dx, dy;
		dx = x - vec2.x;
		dy = y - vec2.y;

		return std::sqrt(dx * dx + dy * dy);
	}

	Vector2 toVector2() {
		return {(float)x, (float)y};
	}

};


