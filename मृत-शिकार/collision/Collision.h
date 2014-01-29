#pragma once
#include "../graphics/stdinc.h"

class Box
{
private:
	glm::vec3 m_center, m_extents;
public:
	Box() {}
	Box(glm::vec3 center, glm::vec3 extents) : m_center(center), m_extents(extents) {}

	const glm::vec3& GetCenter() { return m_center; }
	const glm::vec3& GetExtents() { return m_extents; }
};

class Ray
{
private:
	glm::vec3 m_origin, m_direction;
public:
	Ray() {}
	Ray(glm::vec3 origin, glm::vec3 direction) : m_origin(origin), m_direction(direction) {}
};

struct BoundVolume
{
	Box m_parent;
	std::vector<Box> m_children;
};