#pragma once
#include "../graphics/graphics.h"

class Camera
{
protected:
	glm::mat4 m_view, m_projection;

public:
	virtual void UpdateView(double deltaTime);
	virtual void UpdateProjection(float width, float height);

	const glm::mat4& GetView()			{ return m_view; }
	const glm::mat4& GetProjection()	{ return m_projection; }
};