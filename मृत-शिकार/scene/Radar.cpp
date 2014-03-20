#include "Radar.h"

void Radar::Initialize(Sprite * sprite, glm::vec2 position, bool animate) {
	Unit2d::Initialize(sprite, position, animate);
	m_sprZombie.LoadSprite("radar_zombie.png", 5, 5);
	m_sprPeople.LoadSprite("radar_people.png", 5, 5);

}

void Radar::Draw()
{
	if (!m_visible) return;
	m_sprite->DrawSprite(m_sprAnim, m_position.x, m_position.y);

	for (unsigned int i = 0; i < m_scene->GetUnits().size(); ++i)
	{
		Unit * unit = m_scene->GetUnits()[i];
		if (unit->GetTag() != 2 && unit->GetTag() != 10) continue;
		glm::vec3 vector = unit->GetBoundCenter() - m_player->GetBoundCenter();
		if (glm::dot(vector, vector) > (m_radius * m_radius)) continue;

		float ratio = m_sprite->GetWidth() / 2.0f / m_radius;
		
		vector = vector * glm::mat3(m_player->GetOrient());
		glm::vec2 pos(m_sprite->GetWidth() / 2.0f - vector.x*ratio + m_position.x,
			m_sprite->GetWidth() / 2.0f - vector.z*ratio + m_position.y);

		if (unit->GetTag() == 2)
			m_sprZombie.DrawSprite(0, pos.x, pos.y);
		else
			m_sprPeople.DrawSprite(0, pos.x, pos.y);
	}
}