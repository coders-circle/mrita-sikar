#pragma once
#include "../graphics/graphics.h"

class Billboard
{
protected:
	Sprite * m_sprite;
	SpriteAnimation m_sprAnim;
	bool m_animate;

	glm::vec3 m_position;
public:
	Billboard(const glm::vec3 &position = glm::vec3());

	virtual void Initialize(Sprite * sprite, glm::vec3 position = glm::vec3(), bool animate = false) { m_sprite = sprite; m_position = position; m_animate = animate; }
	virtual void CleanUp() {}

	void Animate(bool animate) { m_animate = animate; }
	void SetImageId(unsigned int index) { m_sprAnim.imageid = index; }

	virtual void Update(double deltaTime) { if (m_animate) m_sprite->Animate(m_sprAnim, deltaTime); }
	virtual void Draw()
	{
		m_sprite->DrawBillboard(m_sprAnim, glm::translate(glm::mat4(), m_position));
	}

	void SetPosition(const glm::vec3 &position) { m_position = position; }
	const glm::vec3 &GetPosition() { return m_position; }

};

