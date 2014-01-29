#include "Camera.h"

void Camera::UpdateView(double deltaTime)
{
	m_view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::UpdateProjection(float width, float height)
{
	m_projection = glm::perspective(45.0f, width / height, 0.1f, 10000.0f);
}