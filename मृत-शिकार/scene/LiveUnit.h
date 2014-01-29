#pragma once
#include "Unit.h"

class LiveUnit : public Unit
{
protected:
	glm::vec3 m_velocity;
	glm::mat3 m_orient;			// the 3x3 matrix is used for collision detection for object oriented bounding box
public:
	LiveUnit();
	LiveUnit(const glm::vec3 &position);
	virtual ~LiveUnit() = 0 {}

	virtual void SetVelocity(const glm::vec3& velocity) { m_velocity = velocity; }
	const glm::vec3 &GetVelocity() { return m_velocity; }

	virtual void SetPosition(const glm::vec3& position) { m_position = position; }
	const glm::vec3 &GetPosition() { return m_position; }

	const glm::mat3 &GetOrient() { return m_orient; }

	virtual void Draw();

};