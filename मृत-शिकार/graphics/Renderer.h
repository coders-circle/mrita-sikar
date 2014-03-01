#pragma once

#include "Techniques.h"
#include "Window.h"

class Renderer
{
private:
	Window * m_window;

	Techniques m_techniques;
	void InitShaders();

	glm::mat4 m_projection, m_viewProjection3d, m_projection2d, m_viewProjectionBB, m_lightViewProjection;
public:
	enum Passes { SHADOW_PASS, NORMAL_PASS };

	Renderer(Window* window);
	void Resize(float width, float height, const glm::mat4 &projection);
	void UpdateView(const glm::mat4 &view)
	{
		m_viewProjection3d = m_projection * view;
		m_viewProjectionBB = m_projection * glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), view[3]);
	}

	void UpdateShadowMatrix(const glm::mat4 &lightViewProjection)
	{
		m_lightViewProjection = lightViewProjection;
	}

	void BeginRender(unsigned int pass)
	{
		switch (pass)
		{
		case NORMAL_PASS:
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			break;
		}
	}

	void EndRender(unsigned int pass)
	{
		switch (pass)
		{
		case NORMAL_PASS:
			m_window->SwapBuffers();
			break;
		};
	}

	void Initialize();
	void CleanUp();

	Techniques& GetTechniques(){ return m_techniques; }
	const glm::mat4& GetViewProjection3d() { return m_viewProjection3d; }
	const glm::mat4& GetViewProjectionBB() { return m_viewProjectionBB; }
	const glm::mat4& GetProjection2d() { return m_projection2d; }
	const glm::mat4& GetLightViewProjection() { return m_lightViewProjection; }
};

