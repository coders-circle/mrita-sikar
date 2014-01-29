#include "stdinc.h"
#include "utils.h"
#include "Renderer.h"

Renderer::Renderer(Window* window) : m_window(window)
{
}

void Renderer::Resize(float width, float height, const glm::mat4 &projection)
{
	if (width == 0 || height == 0) return;
	glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

	m_projection = projection;
	m_projection2d = glm::ortho(0.0f, width, height, 0.0f);
}

void Renderer::UpdateView(const glm::mat4 &view)
{
	m_viewProjection3d = m_projection * view;
	m_viewProjectionBB = m_projection * glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), view[3]);
}

void Renderer::BeginRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndRender()
{
	m_window->SwapBuffers();
}

void Renderer::Initialize()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.396f, 0.612f, 0.937f, 1.0f);

	InitShaders();
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
	};

	std::vector<ProgramInfo> programs = { 
		{ { 0, 1 }, &m_techniques.skin.program },
		{ { 1, 2 }, &m_techniques.normal3D.program },
		{ { 3, 4 }, &m_techniques.sprite.program }
	};
	CreatePrograms(shaders, programs);

	m_techniques.normal3D.Init();
	m_techniques.skin.Init();
	m_techniques.sprite.Init();
}
