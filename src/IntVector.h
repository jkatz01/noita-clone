#include <cmath>

#pragma once
class IntVector {
public:

	int x, y;

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

	IntVector difference(IntVector vec2) {
		int dx, dy;
		dx = x - vec2.x;
		dy = y - vec2.y;
		return {dx, dy};
	}
};


