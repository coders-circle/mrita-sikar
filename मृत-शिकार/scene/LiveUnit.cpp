#include "LiveUnit.h"

LiveUnit::LiveUnit()
{
	m_liveUnit = true;
}

LiveUnit::LiveUnit(const glm::vec3 &position) : Unit(position)
{
	m_liveUnit = true;
}

void LiveUnit::Draw()
{
	if (m_model)
	{
		m_model->SetTransform(glm::translate(glm::mat4(), m_position) * m_orient);
		m_model->Draw();
	}
}