#pragma once
#include "../graphics/graphics.h"

class Unit2d
{
protected:
	Sprite * m_sprite;
	SpriteAnimation m_sprAnim;
	bool m_animate;

	glm::vec2 m_position;
public:
	Unit2d(const glm::vec2 &position = glm::vec2());

	virtual void Initialize(Sprite * sprite, glm::vec2 position = glm::vec2(), bool animate = false) { m_sprite = sprite; m_position = position; m_animate = animate;  }
	virtual void CleanUp() {}

	void Animate(bool animate) { m_animate = animate; }
	void SetImageId(unsigned int index) { m_sprAnim.imageid = index; }

	virtual void Update(double deltaTime) { if (m_animate) m_sprite->Animate(m_sprAnim, deltaTime); }
	virtual void Draw()
	{
		m_sprite->DrawSprite(m_sprAnim, m_position.x, m_position.y);
	}

	void SetPosition(const glm::vec2 &position) { m_position = position; }
	const glm::vec2 &GetPosition() { return m_position; }

};

