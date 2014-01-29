#include "Unit2d.h"


Unit2d::Unit2d()
{
}

Unit2d::Unit2d(const glm::vec2 &position) : m_position(position)
{
}

void Unit2d::Draw()
{
	m_sprite->DrawSprite(0, m_position.x, m_position.y);
}

