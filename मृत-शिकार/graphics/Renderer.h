#pragma once

#include "Techniques.h"
#include "Window.h"

class Renderer
{
private:
	Window * m_window;

	Techniques m_techniques;
	void InitShaders();

	glm::mat4 m_projection, m_viewProjection3d, m_projection2d, m_viewProjectionBB;
	void(*m_render)(int pass);
public:
	Renderer(Window* window);
	void Resize(float width, float height, const glm::mat4 &projection);
	void UpdateView(const glm::mat4 &view);
	void BeginRender();
	void EndRender();
	void Initialize();
	void CleanUp();

	Techniques& GetTechniques(){ return m_techniques; }
	glm::mat4& GetViewProjection3d() { return m_viewProjection3d; }
	glm::mat4& GetViewProjectionBB() { return m_viewProjectionBB; }
	glm::mat4& GetProjection2d() { return m_projection2d; }
};

