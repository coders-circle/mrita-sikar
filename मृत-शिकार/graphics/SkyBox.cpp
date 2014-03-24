#include "stdinc.h"
#include "TextureLoader.h"
#include "SkyBox.h"


SkyBox::SkyBox(Renderer * renderer) : m_renderer(renderer), m_loaded(false)
{
}

void SkyBox::Initialize()
{
	if (m_loaded) CleanUp();
	m_loaded = true;
	
	const float extent = 2000.0f;
	GLfloat vertices[] = {
		-extent, extent, extent,
		-extent, -extent, extent,
		extent, -extent, extent,
		extent, extent, extent,
		-extent, extent, -extent,
		-extent, -extent, -extent,
		extent, -extent, -extent,
		extent, extent, -extent,
	};
	GLushort indices[] = {
		0, 1, 2, 3,
		3, 2, 6, 7,
		7, 6, 5, 4,
		4, 5, 1, 0,
		0, 3, 7, 4,
		1, 2, 6, 5,
	};

	m_texture = LoadCubeMap(
		"textures\\skybox_zn.tga", "textures\\skybox_zp.tga",
		"textures\\skybox_yp.tga", "textures\\skybox_yn.tga",
		"textures\\skybox_xn.tga", "textures\\skybox_xp.tga"
		);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(NumBuffers, m_buffers);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[IBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void SkyBox::CleanUp()
{
	if (!m_loaded) return;
	glDeleteBuffers(NumBuffers, m_buffers);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteTextures(1, &m_texture);
	m_loaded = false;
}

void SkyBox::Draw(const glm::vec3 &center)
{
	if (!m_renderer) return;
	if (!m_loaded) return;
	if (m_texture <= 0) return;

	Techniques &techniques = m_renderer->GetTechniques();

	glDisable(GL_CULL_FACE);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glUseProgram(techniques.sky.program);
	glUniformMatrix4fv(techniques.sky.mvp, 1, GL_FALSE, glm::value_ptr(m_renderer->GetViewProjection3d() * glm::translate(glm::mat4(), center) * glm::scale(glm::mat4(), glm::vec3(10.0f))));
	glUniform1i(techniques.sky.texture_sample, 0);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[IBO]);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);

	glEnable(GL_CULL_FACE);
}
