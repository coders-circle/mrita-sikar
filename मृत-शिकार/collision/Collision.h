#pragma once
#include "../graphics/stdinc.h"

#define SIGN(x) (((x)<0?-1.0f:1.0f))

template <typename T>
inline void Swap(T &x, T &y) { T temp; temp = x; x = y; y = temp; };

class Box
{
private:
	glm::vec3 m_center, m_extents;
public:
	Box() {}
	Box(glm::vec3 center, glm::vec3 extents) : m_center(center), m_extents(extents) {}

	const glm::vec3& GetCenter() const
	{ return m_center; }
	const glm::vec3& GetExtents() const
	{ return m_extents; }

	bool IntersectBox(const Box &box2, glm::vec3 * out = NULL);
	bool IntersectBox(const glm::mat3 &orient1, const Box &box2, const glm::mat3 &orient2, glm::vec3 * out = NULL);
};

class Ray
{
private:
	glm::vec3 m_origin, m_direction;
public:
	Ray() {}
	Ray(glm::vec3 origin, glm::vec3 direction) : m_origin(origin), m_direction(direction) {}
	
	const glm::vec3& GetOrigin() const
	{ return m_origin; }
	const glm::vec3& GetDirection() const
	{ return m_direction; }

	bool IntersectBox(const Box &box, float &tmin);
	bool IntersectBox(const Box &box, const glm::mat3 &orient, float &tmin)
	{
		Ray newray(m_origin, m_direction * glm::transpose(orient));
		return newray.IntersectBox(box, tmin);
	}
};

struct BoundVolume
{
	Box parent;
	std::vector<Box> children;
};