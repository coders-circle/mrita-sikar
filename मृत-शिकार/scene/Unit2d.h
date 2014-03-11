#pragma once
#include "../graphics/graphics.h"

class Unit2d
{
protected:
	Sprite * m_sprite;

	glm::vec2 m_position;
public:
	Unit2d();
	Unit2d(const glm::vec2 &position);

	virtual void Initialize(Sprite * sprite, glm::vec2 position = glm::vec2()) { m_sprite = sprite; m_position = position; }
	virtual void CleanUp() {}

	virtual void Update(double deltaTime) {}
	virtual void Draw()
	{
		m_sprite->DrawSprite(0, m_position.x, m_position.y);
	}

	void SetPosition(const glm::vec2 &position) { m_position = position; }
	const glm::vec2 &GetPosition() { return m_position; }

};

