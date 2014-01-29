#pragma once
#include "LiveUnit.h"
#include "Camera.h"

class TPCamera : public Camera
{
private:
	LiveUnit * m_target;
	float m_distance;
public:
	void UpdateView(double deltaTime)
	{
		glm::vec3 tpos = m_target->GetPosition();
		tpos = glm::vec3(tpos.x, tpos.y + 50.0f, tpos.z);
		m_view = glm::lookAt(
			tpos - (glm::vec3)m_target->GetOrient()[2] * m_distance,
			tpos,						
			(glm::vec3)m_target->GetOrient()[1]
			);
	}
	void SetParameters(LiveUnit * target, float distance)
	{
		m_target = target; m_distance = distance;
	}
};

