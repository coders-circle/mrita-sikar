#include "LiveUnit.h"

LiveUnit::LiveUnit()
{}

LiveUnit::LiveUnit(const glm::vec3 &position) : Unit(position)
{}

void LiveUnit::Draw()
{
	if (m_model)
	{
		m_model->SetTransform(glm::translate(glm::mat4(), m_position) * m_orient);
		m_model->Draw();
	}
}