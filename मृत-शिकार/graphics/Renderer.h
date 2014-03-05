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
	unsigned int m_pass;

	GLuint m_depthFBO, m_depthTexture;
	float m_width, m_height;
public:
	enum Passes { SHADOW_PASS, NORMAL_PASS };

	Renderer(Window* window);
	void Resize(float width, float height, const glm::mat4 &projection);
	void UpdateView(const glm::mat4 &view)
	{
		m_viewProjection3d = m_projection * view;
		m_viewProjectionBB = m_projection * glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), view[3]);
	}

	void UpdateLightMatrix(const glm::mat4 &lightViewProjection)
	{
		m_lightViewProjection = lightViewProjection;
	}

	void SetupLightMatrix(const glm::vec3 &lightDirection, const glm::vec3 &target, float lrLimit, float tbLimit, float minZ, float maxZ)
	{
		m_lightViewProjection = glm::ortho(-lrLimit, lrLimit, -tbLimit, tbLimit, minZ, maxZ)
						* glm::lookAt(target-lightDirection, target, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void BeginRender(unsigned int pass);

	void EndRender()
	{
		//if (m_pass == NORMAL_PASS)
			m_window->SwapBuffers();
	}

	void Initialize();
	void CleanUp();

	Techniques& GetTechniques(){ return m_techniques; }
	const glm::mat4& GetViewProjection3d() { return m_viewProjection3d; }
	const glm::mat4& GetViewProjectionBB() { return m_viewProjectionBB; }
	const glm::mat4& GetProjection2d() { return m_projection2d; }
	const glm::mat4& GetLightViewProjection() { return m_lightViewProjection; }

	unsigned int GetRenderPass() { return m_pass; }
	GLuint GetDepthTexture() { return m_depthTexture; }

	
};

