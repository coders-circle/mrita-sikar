/*
RENDERER.CPP
-----------------------------
*/

#include "stdinc.h"
#include "utils.h"
#include "Renderer.h"

Renderer::Renderer(Window* window) : m_window(window)
{
}

void Renderer::Resize(float width, float height, const glm::mat4 &projection)
{
	if (width == 0 || height == 0) return;
	//glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	m_projection = projection;
	m_projection2d = glm::ortho(0.0f, width, height, 0.0f);

	m_width = width;
	m_height = height;
}

void Renderer::Initialize()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.396f, 0.612f, 0.937f, 1.0f);

	InitShaders();

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &m_depthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);

	glDrawBuffer(GL_NONE); // No color buffer is drawn to.
	glReadBuffer(GL_NONE);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR CREATING DEPTH MAP" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CleanUp()
{
	glDeleteProgram(m_techniques.normal3D.program);
	glDeleteProgram(m_techniques.skin.program);
}

void Renderer::InitShaders()
{
	std::vector<ShaderInfo> shaders = {
		{ GL_VERTEX_SHADER, "shaders\\vs_skin.glsl" },
		{ GL_FRAGMENT_SHADER, "shaders\\fs_normal3d.glsl" },
		{ GL_VERTEX_SHADER, "shaders\\vs_normal3d.glsl" },
		{ GL_FRAGMENT_SHADER, "shaders\\fs_sprite.glsl" },
		{ GL_VERTEX_SHADER, "shaders\\vs_sprite.glsl" },
		{ GL_VERTEX_SHADER, "shaders\\vs_depthmap_skin.glsl" },
		{ GL_FRAGMENT_SHADER, "shaders\\fs_depthmap.glsl" },
		{ GL_VERTEX_SHADER, "shaders\\vs_depthmap_normal.glsl" },
	};

	std::vector<ProgramInfo> programs = { 
		{ { 0, 1 }, &m_techniques.skin.program },
		{ { 1, 2 }, &m_techniques.normal3D.program },
		{ { 3, 4 }, &m_techniques.sprite.program },
		{ { 5, 6 }, &m_techniques.depthMapSkin.program },
		{ { 6, 7 }, &m_techniques.depthMapNormal.program },
	};
	CreatePrograms(shaders, programs);

	m_techniques.normal3D.Init();
	m_techniques.skin.Init();
	m_techniques.sprite.Init();
	m_techniques.depthMapNormal.Init();
	m_techniques.depthMapSkin.Init();
}

void Renderer::BeginRender(unsigned int pass)
{
	m_pass = pass;
	switch (pass)
	{
	case NORMAL_PASS:
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glCullFace(GL_BACK);
		glViewport(0, 0, static_cast<int>(m_width), static_cast<int>(m_height));
		break;
	case SHADOW_PASS:
		glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		glCullFace(GL_FRONT);
		glViewport(0, 0, 4096, 4096);
		break;
	}
}
