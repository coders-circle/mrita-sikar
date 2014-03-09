#pragma once

/*
TEXTURELOADER.H
-----------------------------
*/

#include "SOIL.h"

// Uses soil library to load a texture
inline unsigned int LoadTexture(std::string filename)
{
	unsigned int texture = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	if (texture == 0)
		std::cout << "ERROR loading texture: " << filename << std::endl << SOIL_last_result() << std::endl;
	return texture;
}

// Use soil library to load 6 textures as cubemap
inline unsigned int LoadCubeMap(std::string xPosFile, std::string xNegFile, std::string yPosFile, std::string yNegFile, std::string zPosFile, std::string zNegFile)
{
	unsigned int texture = SOIL_load_OGL_cubemap(
		xPosFile.c_str(), xNegFile.c_str(),
		yPosFile.c_str(), yNegFile.c_str(),
		zPosFile.c_str(), zNegFile.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS
		);

	if (texture == 0)
		std::cout << "ERROR loading cubemap "  << std::endl << SOIL_last_result() << std::endl;
	return texture;
}