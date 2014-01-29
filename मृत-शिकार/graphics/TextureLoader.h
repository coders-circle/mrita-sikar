#pragma once

#include "SOIL.h"

inline unsigned int LoadTexture(std::string filename)
{
	unsigned int texture = SOIL_load_OGL_texture(filename.data(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	if (texture == 0)
		std::cout << "ERROR loading texture: " << SOIL_last_result() << std::endl;
	return texture;
}