#pragma once
#include "LiveUnit.h"
#include "Camera.h"

class TPCamera : public Camera
{
private:
	LiveUnit * m_target;
	float m_distance;

	glm::mat4 m_orient;
	float m_totalYRot;		//limit yrotation
public:
	void UpdateView(double deltaTime)
	{
		m_orient = glm::rotate(m_target->GetOrient(), m_totalYRot, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 tpos = m_target->GetPosition();
		tpos = glm::vec3(tpos.x, tpos.y + 50.0f, tpos.z);
		m_view = LookAt(
			tpos - (glm::vec3)m_orient[2] * m_distance,
			tpos,						
			(glm::vec3)m_orient[1]
			);
	}

	void RotateY(float deltaY)
	{
		float prevRotY = m_totalYRot;
		m_totalYRot += deltaY;
		if (m_totalYRot > 30.0f) { m_totalYRot = 30.0f; }
		else if (m_totalYRot < -40.0f) { m_totalYRot = -40.0f; }
	}

	void Initialize(LiveUnit * target, float distance)
	{
		m_totalYRot = 0.0f;
		RotateY(10.0f);
		m_target = target; m_distance = distance;
	}

	void SetDistance(float distance)
	{
		m_distance = distance;
	}

	float GetDistance() { return m_distance; }

	glm::vec3 GetPosition() {
		glm::vec3 tpos = m_target->GetPosition();
		tpos = glm::vec3(tpos.x, tpos.y + 50.0f, tpos.z);
		return tpos - (glm::vec3)m_orient[2] * m_distance;
	}
};

