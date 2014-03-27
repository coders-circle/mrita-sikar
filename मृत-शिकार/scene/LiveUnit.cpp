#include "LiveUnit.h"

LiveUnit::LiveUnit(const glm::vec3 &position) : Unit(position)
{
	m_liveUnit = true;
	m_health = 100;
}
