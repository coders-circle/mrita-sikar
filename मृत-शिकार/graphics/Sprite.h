#pragma once

#include "Renderer.h"

struct SpriteAnimation
{
	SpriteAnimation(){ time = 0.0; imageid = 0; }
	unsigned imageid;
	double time;
};

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

	void Animate(SpriteAnimation &spriteAnimation, double deltaTime);
	void LoadSprite(std::string filename, float width, float height, float offsetX = 0.0f, float offsetY = 0.0f, int numCols = 1, int numRows = 1, double animationSpeed = 1.0);
	void DrawSprite(unsigned imageid, float posX, float posY);
	void DrawBillboard(unsigned imageid, const glm::mat4 &transform);

	void DrawSprite(const SpriteAnimation &spriteAnimation, float posX, float posY) 
	{ DrawSprite(spriteAnimation.imageid, posX, posY); }
	void DrawBillboard(const SpriteAnimation &spriteAnimation, const glm::mat4 &transform) 
	{ DrawBillboard(spriteAnimation.imageid, transform); }
};

