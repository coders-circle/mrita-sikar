#pragma once
#include "LiveUnit.h"
#include "Camera.h"

class TPCamera : public Camera
{
private:
	LiveUnit * m_target;
	float m_distance;

	glm::mat4 m_orient;
	float m_totalYRot;		//record to limit yrotation
public:
	void UpdateView(double deltaTime)
	{
		glm::vec3 tpos = m_target->GetPosition();
		tpos = glm::vec3(tpos.x, tpos.y + 50.0f, tpos.z);
		m_view = LookAt(
			tpos - (glm::vec3)m_orient[2] * m_distance,
			tpos,						
			(glm::vec3)m_orient[1]
			);
	}

	void RotateX(float deltaX)
	{
		m_orient = glm::rotate(glm::mat4(), -deltaX, glm::vec3(0.0f, 1.0f, 0.0f)) * m_orient;
	}

	void RotateY(float deltaY)
	{
		m_totalYRot += deltaY;
		if (m_totalYRot > 35.0f) m_totalYRot = 35.0f;
		else if (m_totalYRot < -40.0f) m_totalYRot = -40.0f;
		else m_orient = glm::rotate(m_orient, deltaY, glm::vec3(1.0f, 0.0f, 0.0f));
	}


	void Initialize(LiveUnit * target, float distance)
	{
		m_totalYRot = 0.0f;
		RotateY(10.0f);
		m_orient = target->GetOrient();
		m_target = target; m_distance = distance;
	}

	glm::vec3 GetPosition() {
		glm::vec3 tpos = m_target->GetPosition();
		tpos = glm::vec3(tpos.x, tpos.y + 50.0f, tpos.z);
		return tpos - (glm::vec3)m_orient[2] * m_distance;
	}
};

