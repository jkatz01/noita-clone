#pragma once
#include <cstdint>
struct BufferPixelRGBAInt32 {
	uint32_t id; //R32UI
	uint32_t unused1; //G32UI
	uint32_t unused2; //B32UI
	uint32_t unused3; //A32UI
};
struct BufferPixelRGBAFloat32 {
	float r; //R32F
	float g; //G32F
	float b; //B32F
	float a; //A32F
};
//do not remove the unused, they are used for padding