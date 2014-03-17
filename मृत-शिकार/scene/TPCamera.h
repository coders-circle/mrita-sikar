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

	bool m_isShaking; float m_amplitude; float m_timeElapsed;
public:
	void UpdateView(double deltaTime)
	{
		m_orient = glm::rotate(m_target->GetOrient(), m_totalYRot, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 tpos = m_target->GetPosition();
		tpos = glm::vec3(tpos.x, tpos.y + 50.0f, tpos.z);

		glm::vec3 eye = tpos - glm::vec3(m_orient[2]) * m_distance;

		if (m_isShaking)
		{
			// Shaking is modeled by damped oscillation
			// y = A * e^(-bt) * sin(wt)
			// since exponential is expensive computation, 
			// we just decrease the overall amplitude by fixed quantity each time

			m_timeElapsed += static_cast<float>(deltaTime);
			// decrease amplitude by 5.0f per unit time
			m_amplitude -= static_cast<float>(deltaTime) * 5.0f;
			// if amplitude is below certain range, stop oscillation
			if (m_amplitude < 0.002f) { m_isShaking = false; }

			// Get a random direction to oscillate
			glm::vec3 dir = glm::normalize(glm::vec3(rand() % 20, rand() % 20, 0.0f));
			
			// Dislace eye by A*sin(wt) along 'dir'
			eye += dir * m_amplitude * glm::sin(m_timeElapsed*30.0f);;
		}
		m_view = LookAt(eye, tpos, glm::vec3(m_orient[1]));
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
		m_isShaking = false;
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

	void Shake() { m_isShaking = true; m_amplitude = 3.0f; m_timeElapsed = 0.0f; }
};

