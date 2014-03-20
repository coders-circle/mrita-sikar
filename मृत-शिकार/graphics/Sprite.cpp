/*
SPRITE.CPP
-----------------------------
*/

#include "stdinc.h"
#include "TextureLoader.h"
#include "Sprite.h"

Sprite::Sprite(Renderer * renderer) : m_renderer(renderer), m_loaded(false)
{
}

void Sprite::CleanUp()
{
	if (!m_loaded) return;
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteTextures(1, &m_texture);
	m_loaded = false;
}

void Sprite::Animate(SpriteAnimation &spriteAnimation, double deltaTime, bool loop, bool * end)
{
	spriteAnimation.time += deltaTime;
	if (end) *end = false;
	if (spriteAnimation.time >= m_deltaTime)
	{
		spriteAnimation.time = 0;
		++spriteAnimation.imageid;
		if (end && spriteAnimation.imageid >= static_cast<unsigned int>(m_numCols * m_numRows)) *end = true;
		if (loop) spriteAnimation.imageid %= m_numCols * m_numRows;
	}
}

void Sprite::LoadSprite(std::string filename, float width, float height, float offsetX, float offsetY, int numCols, int numRows, double animationSpeed)
{
	if (m_loaded) CleanUp();
	m_loaded = true;
	
	m_deltaTime = 1.0/animationSpeed;
	m_width = width; m_height = height; 
	m_numCols = numCols; m_numRows = numRows;

	struct Vertex{ glm::vec2 pos, tex; };
	float u = 1.0f / numCols, v = 1.0f / numRows;
	std::vector<Vertex> vertices = {
		{ glm::vec2(-offsetX, -offsetY), glm::vec2(0.0f, 1.0f) },
		{ glm::vec2(-offsetX, height - offsetY), glm::vec2(0.0f, 1.0f - v) },
		{ glm::vec2(width - offsetX, height - offsetY), glm::vec2(u, 1.0f - v) },
		{ glm::vec2(width - offsetX, -offsetY), glm::vec2(u, 1.0f) },
	};
	m_bbTransform = glm::scale(glm::mat4(), glm::vec3(1.0f / 8.0f, -1.0f / 8.0f, 0.0f)) *
		glm::translate(glm::mat4(), glm::vec3(-m_width / 2, -m_height / 2, 0.0f));

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, pos));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tex));

	m_texture = LoadTexture("sprites\\" + filename);
}

void Sprite::DrawSprite(unsigned imageid, float posX, float posY, float scale, float visibility)
{
	if (!m_renderer) return;
	if (!m_loaded) return;

	glm::vec2 uv((float)(imageid % m_numCols) / (float)m_numCols, - (float)(imageid / m_numCols) / (float)m_numRows);

	Techniques &techniques = m_renderer->GetTechniques();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glUseProgram(techniques.sprite.program);
	glUniformMatrix4fv(techniques.sprite.mvp, 1, GL_FALSE, glm::value_ptr(m_renderer->GetProjection2d() * glm::translate(glm::mat4(), glm::vec3(posX, posY, 0.0f)) * glm::scale(glm::mat4(), glm::vec3(scale))));
	glUniform2fv(techniques.sprite.uv, 1, glm::value_ptr(uv));
	glUniform1i(techniques.sprite.texture_sample, 0);
	glUniform1f(techniques.sprite.visiblity, visibility);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_QUADS, 0, 4);
}


void Sprite::DrawBillboard(unsigned imageid, const glm::mat4 &transform, float visibility)
{
	if (!m_renderer) return;
	if (!m_loaded) return;
	glm::vec2 uv((float)(imageid % m_numCols) / (float)m_numCols, -(float)(imageid / m_numCols) / (float)m_numRows);

	Techniques &techniques = m_renderer->GetTechniques();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glUseProgram(techniques.sprite.program);
	//glm::mat4 resTransform = m_renderer->GetBillboardRotation();
	//resTransform[3] = transform[3];
	glUniformMatrix4fv(techniques.sprite.mvp, 1, GL_FALSE,
		glm::value_ptr(m_renderer->GetViewProjection3d() * transform * m_renderer->GetBillboardRotation() * m_bbTransform));
	glUniform2fv(techniques.sprite.uv, 1, glm::value_ptr(uv));
	glUniform1i(techniques.sprite.texture_sample, 0);
	glUniform1f(techniques.sprite.visiblity, visibility);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_QUADS, 0, 4);
}

