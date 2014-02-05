#include "Unit.h"

Unit::Unit(const glm::vec3 &position) : m_model(NULL), m_position(position), m_dead(false), m_tag(0), m_scene(NULL)
{
	m_liveUnit = false;
}