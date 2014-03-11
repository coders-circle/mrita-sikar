#pragma once
#include "Billboard.h"
class Blood : public Billboard
{
private:
	bool m_draw;
public:
	Blood(const glm::vec3& position = glm::vec3());

	void Start(const glm::vec3& position) {
		m_draw = true; 
		m_position = position; 
		m_animate = true;
		m_sprAnim.imageid = 0; m_sprAnim.time = 0.0;
	}
	void Update(double deltaTime)
	{
		if (m_draw)
		{
			bool end; m_sprite->Animate(m_sprAnim, deltaTime * 10, false, &end); if (end) m_draw = false;
		}
	}
	void Draw() {
		if (m_draw)
			Billboard::Draw();
	}
};

