#pragma once
#include "../graphics/graphics.h"

class Camera
{
protected:
	glm::mat4 m_view, m_projection;
	Frustum m_frustum;

	glm::mat4 LookAt(glm::vec3 eye, glm::vec3 at, glm::vec3 up)
	{
		m_frustum.SetCamDef(eye, at, up);
		return glm::lookAt(eye, at, up);
	}

	glm::mat4 Perspective(float angle, float aspect, float nearD, float farD)
	{
		m_frustum.SetCamInternals(angle, aspect, nearD, farD);
		return glm::perspective(angle, aspect, nearD, farD);
	} 
public:
	virtual void UpdateView(double deltaTime)
	{
		m_view = LookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	virtual void UpdateProjection(float width, float height)
	{
		m_projection = Perspective(45.0f, width / height, 0.1f, 30000.0f);
	}
	
	const glm::mat4& GetView()			{ return m_view; }
	const glm::mat4& GetProjection()	{ return m_projection; }

	bool IntersectBox(const Box &box)
	{
		return m_frustum.BoxInFrustum(box);
	}

	virtual glm::vec3 GetPosition() { return glm::vec3(); }
};