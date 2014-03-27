#pragma once

/*
RENDERER.H
-----------------------------
*/

#include "Techniques.h"
#include "Window.h"

// Main heart of the graphics system
// The renderer is responsible for initializing OpenGL
// and managing shaders, shadow depth-map, view-projection tranformations
// and is used to begin and end rendering a frame
class Renderer
{
private:
	Window * m_window;

	Techniques m_techniques;
	void InitShaders();

	glm::mat4 m_projection, m_viewProjection3d, m_projection2d, m_lightViewProjection, m_rotationBB;
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
		m_rotationBB = glm::mat4(glm::mat3(glm::inverse(view)));
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
	const glm::mat4& GetBillboardRotation() { return m_rotationBB; }
	const glm::mat4& GetProjection2d() { return m_projection2d; }
	const glm::mat4& GetLightViewProjection() { return m_lightViewProjection; }

	unsigned int GetRenderPass() { return m_pass; }
	GLuint GetDepthTexture() { return m_depthTexture; }

	void DisableDepth() { glDisable(GL_DEPTH_TEST); }
	void EnableDepth() { glEnable(GL_DEPTH_TEST); }
	float GetHeight(){ return m_height; }
	float GetWidth(){ return m_width; }
};

