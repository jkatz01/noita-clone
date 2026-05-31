#pragma once
#include <raylib.h>
#include <rlgl.h>
#include <glad/glad.h>
#include "IntVector.hpp"
#include "SandTile.hpp"
#include "BufferPixels.hpp"
#pragma warning( disable : 26495 )

class SandWorldRenderer
{
	Shader worldRenderer;

	Texture2D gridBuffer0;

	//unused
	Texture2D colorMapBuffer;

	RenderTexture2D renderTexture;

	IntVector tileOrigin = IntVector(0, 0); //top left

	const int TILE_SIZE = 100;//TODO get the truth from tile_size in SandTile.cpp, will cause future bug if we change the tile size in tile only

	int bufferWidth, bufferHeight;

	int renderMargin = 10;
	IntVector renderOffset = IntVector(renderMargin, renderMargin); //essentially 0,0
	bool init = false;

public:
	SandWorldRenderer() {
		Init();
	}

	void Init() {

		worldRenderer = LoadShader(0, "src/tile_fragment_shader.glsl");

		int screenWidth = GetScreenWidth();
		int screenHeight = GetScreenHeight();

		//grid buffers need to hold whole chunks

		int xCount = (screenWidth / 100) + 1; //since int div floors, +1 makes sure we are over the amount
		int yCount = (screenHeight / 100) + 1;
		//These are the tile counts,


		Image img = GenImageColor(200, 200, BLANK); // fully transparent image
		// Alternatively, manually create image data:
		// Image img = GenImageColor(200, 200, (Color){0, 0, 0, 0});

		// Convert image to GPU texture
		Texture2D texture = LoadTextureFromImage(img);




		bufferWidth = xCount * TILE_SIZE, bufferHeight = yCount * TILE_SIZE;
		
		//buffer base data
		std::vector<BufferPixelRGBAFloat32> empty;
		empty.resize(1 * bufferWidth * bufferHeight);

		//construct buffer0
		gridBuffer0 = { 0 };
		gridBuffer0.id = rlLoadTexture(empty.data(), bufferWidth, bufferHeight, RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32, 1);
		gridBuffer0.width = bufferWidth;
		gridBuffer0.height = bufferHeight;
		gridBuffer0.mipmaps = 1;
		gridBuffer0.format = PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;


		/*
		//construct color map
		//TODO change this to be based on a json file
		std::vector<Color> typeColors;
		typeColors.resize(256); 
		typeColors[0] = Color({ 0,0,0,0 });
		typeColors[1] = Color({ 1,0,0,1 });
		typeColors[2] = Color({ 0,1,0,1 });
		typeColors[3] = Color({ 0,0,1,1 });


		colorMapBuffer = { 0 };
		colorMapBuffer.id = rlLoadTexture(typeColors.data(), (int)typeColors.size(), 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
		colorMapBuffer.width = bufferWidth;
		colorMapBuffer.height = bufferHeight;
		colorMapBuffer.mipmaps = 1;
		colorMapBuffer.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
		*/

		//open gl way, not working idk

		//GL_RGBA32UI
		/*

		unsigned int testBuffer;
		glGenTextures(1, &testBuffer);
		glBindTexture(GL_TEXTURE_2D, testBuffer);

		glTexImage2D(GL_TEXTURE_2D, 0,
			GL_RGBA32UI,
			bufferWidth, bufferHeight,
			0,
			GL_RGBA_INTEGER,
			GL_UNSIGNED_INT,
			empty.data());
			*/

		//gridBuffer0 is changed to an unsigned int (GLuint)


		












		//set no scaling blur shit just incase --- didnt work texture vs rendertexture2d idk
		//SetTextureFilter(gridBuffer0, TEXTURE_FILTER_POINT);

		//for post processing shenanigans, glow from offscreen etc

		renderTexture = LoadRenderTexture(screenWidth + (renderMargin * 2), screenHeight + (renderMargin * 2));




		init = true;
	}


	void uploadTileToBuffer(SandTile& tile) {
		if (!init) { Init(); }
		IntVector relativePos = (tile.position - tileOrigin)*TILE_SIZE;
		//std::cout << "updating tile" <<std::endl;
		UpdateTextureRec(gridBuffer0, Rectangle({ (float)relativePos.x, (float)relativePos.y,(float)TILE_SIZE, (float)TILE_SIZE }), tile.buffer0.data());


		//open gl way, not working 
		/*
		glBindTexture(GL_TEXTURE_2D, gridBuffer0);

		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,								// mip level
			relativePos.x, relativePos.y,	// offset in texture
			TILE_SIZE, TILE_SIZE,			// region size
			GL_RGBA_INTEGER,				// format (or GL_RGBA_INTEGER, etc.)
			GL_UNSIGNED_INT,				// type
			tile.buffer0.data()				// pointer to pixel data
		);
		*/

		//std::cout << "done updating tile" << std::endl;
	}

	void moveCamera(int x, int y) {
		renderOffset.x += x;
		renderOffset.y += y;
	}

	void prerender() {
		if (!init) { Init(); }

		BeginTextureMode(renderTexture);

		BeginShaderMode(worldRenderer);
		auto shaderBuffer0 = GetShaderLocation(worldRenderer, "gridBuffer0");
		SetShaderValueTexture(worldRenderer, shaderBuffer0, gridBuffer0);

		DrawRectangle(0, 0, renderTexture.texture.width, renderTexture.texture.height, WHITE);

		EndShaderMode();

		EndTextureMode();


	}

	void render() {
		if (!init) { Init(); }
		if (true) { return; }
		//BeginShaderMode(worldRenderer);
			DrawTexture(renderTexture.texture, 0,0, WHITE);
		//EndShaderMode();
		
	}

};

