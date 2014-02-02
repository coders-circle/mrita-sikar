#pragma once
#include "../graphics/graphics.h"

class Camera
{
protected:
	glm::mat4 m_view, m_projection;

public:
	virtual void Camera::UpdateView(double deltaTime)
	{
		m_view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	virtual void Camera::UpdateProjection(float width, float height)
	{
		m_projection = glm::perspective(45.0f, width / height, 0.1f, 10000.0f);
	}

	const glm::mat4& GetView()			{ return m_view; }
	const glm::mat4& GetProjection()	{ return m_projection; }
};