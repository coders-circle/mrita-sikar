#pragma once

/*
SPRITE.H
-----------------------------
*/

#include "Renderer.h"

// A structure to hold information about current animation state
struct SpriteAnimation
{
	SpriteAnimation(){ time = 0.0; imageid = 0; }
	unsigned imageid;
	double time;
};

// A sprite class represents 2-d image(s) that can be drawn with or without animation
// Can also be used to draw billboards
class Sprite
{
private:
	Renderer * m_renderer;
	unsigned int m_vao, m_vbo;
	unsigned int m_texture;
	bool m_loaded;

	double m_deltaTime;
	float m_width, m_height;
	int m_numCols, m_numRows;
	glm::mat4 m_bbTransform;
public:
	Sprite(Renderer * renderer = NULL);
	void SetRenderer(Renderer * renderer) { m_renderer = renderer; }
	void CleanUp();

	void Animate(SpriteAnimation &spriteAnimation, double deltaTime, bool loop = true, bool * end = NULL);
	void LoadSprite(std::string filename, float width, float height, float offsetX = 0.0f, float offsetY = 0.0f, int numCols = 1, int numRows = 1, double animationSpeed = 1.0);
	void DrawSprite(unsigned imageid, float posX, float posY, float scale = 1.0f, float visibility=1.0f);
	void DrawBillboard(unsigned imageid, const glm::mat4 &transform, float visibility=1.0f);

	void DrawSprite(const SpriteAnimation &spriteAnimation, float posX, float posY, float scale = 1.0f, float visibility=1.0f)
	{ DrawSprite(spriteAnimation.imageid, posX, posY, scale, visibility); }
	void DrawBillboard(const SpriteAnimation &spriteAnimation, const glm::mat4 &transform, float visibility=1.0f)
	{ DrawBillboard(spriteAnimation.imageid, transform, visibility); }

	float GetWidth() { return m_width; }
	float GetHeight() { return m_height; }
};

