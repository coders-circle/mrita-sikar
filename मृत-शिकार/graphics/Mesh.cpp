
#include "stdinc.h"
#include "Mesh.h"

Mesh::Mesh(Renderer * renderer) : m_texture(0), m_loaded(false), m_bones(NULL), m_renderer(renderer)
{}

void Mesh::LoadData(const std::vector<Vertex> &vertices, const std::vector<unsigned short> &indices)
{
	if (m_loaded) CleanUp();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(NumBuffers, m_buffers);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[IBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoords));

	m_numIndices = indices.size();
	m_loaded = true;
	m_texture = 0;
	m_bones = NULL;
}

void Mesh::LoadData(const std::vector<SkinVertex> &vertices, const std::vector<unsigned short> &indices)
{
	if (m_loaded) CleanUp();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(NumBuffers, m_buffers);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkinVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[IBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinVertex), (GLvoid*)offsetof(SkinVertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinVertex), (GLvoid*)offsetof(SkinVertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkinVertex), (GLvoid*)offsetof(SkinVertex, texcoords));

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, sizeof(SkinVertex), (GLvoid*)(offsetof(SkinVertex, weights) + offsetof(WeightInfo, boneids)));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(SkinVertex), (GLvoid*)(offsetof(SkinVertex, weights) + offsetof(WeightInfo, weights)));

	m_numIndices = indices.size();
	m_loaded = true;
	m_texture = 0;
	m_bones = new std::vector<Bone>;
}


const glm::mat4 g_biasMatrix(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
	);
void Mesh::Draw(const glm::mat4 &transform)
{
	if (!m_renderer) return;
	if (!m_loaded) return;

	Techniques &techniques = m_renderer->GetTechniques();

	if (m_renderer->GetRenderPass() == Renderer::NORMAL_PASS)
	{
		const glm::mat4 &pretransform = m_renderer->GetViewProjection3d();
		if (m_texture > 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_renderer->GetDepthTexture());
		}

		if (m_bones)
		{
			glUseProgram(techniques.skin.program);
			glUniformMatrix4fv(techniques.skin.mvp, 1, GL_FALSE, glm::value_ptr(pretransform * transform));
			glUniformMatrix4fv(techniques.skin.model, 1, GL_FALSE, glm::value_ptr(transform));
			glm::mat4 biaslightmvp = g_biasMatrix * m_renderer->GetLightViewProjection() * transform;
			glUniformMatrix4fv(techniques.skin.bias_light_mvp, 1, GL_FALSE, glm::value_ptr(biaslightmvp));
			glUniform1i(techniques.skin.texture_sample, 0);
			glUniform1i(techniques.skin.texture_depthmap, 1);

			glm::mat4 * bonemats = new glm::mat4[m_bones->size()];
			for (unsigned k = 0; k < m_bones->size(); ++k)
				bonemats[k] = (*m_bones)[k].node->combined_transform * (*m_bones)[k].offset;
			glUniformMatrix4fv(techniques.skin.mbones, m_bones->size(), GL_FALSE, glm::value_ptr(*bonemats));	// MODIFIED from (float*)&bonemats[0][0][0]
			delete[] bonemats;
		}
		else
		{
			glUseProgram(techniques.normal3D.program);
			glUniformMatrix4fv(techniques.normal3D.mvp, 1, GL_FALSE, glm::value_ptr(pretransform * transform));
			glUniformMatrix4fv(techniques.normal3D.model, 1, GL_FALSE, glm::value_ptr(transform));
			glm::mat4 biaslightmvp = g_biasMatrix * m_renderer->GetLightViewProjection() * transform;
			glUniformMatrix4fv(techniques.normal3D.bias_light_mvp, 1, GL_FALSE, glm::value_ptr(biaslightmvp));
			glUniform1i(techniques.normal3D.texture_sample, 0);
			glUniform1i(techniques.normal3D.texture_depthmap, 1);
		}
	}
	else if (m_renderer->GetRenderPass() == Renderer::SHADOW_PASS)
	{
		const glm::mat4 &pretransform = m_renderer->GetLightViewProjection();

		if (m_bones)
		{
			glUseProgram(techniques.depthMapSkin.program);
			glUniformMatrix4fv(techniques.depthMapSkin.mvp, 1, GL_FALSE, glm::value_ptr(pretransform * transform));

			glm::mat4 * bonemats = new glm::mat4[m_bones->size()];
			for (unsigned k = 0; k < m_bones->size(); ++k)
				bonemats[k] = (*m_bones)[k].node->combined_transform * (*m_bones)[k].offset;
			glUniformMatrix4fv(techniques.depthMapSkin.mbones, m_bones->size(), GL_FALSE, glm::value_ptr(*bonemats));	
			delete[] bonemats;
		}
		else
		{
			glUseProgram(techniques.depthMapNormal.program);
			glUniformMatrix4fv(techniques.depthMapNormal.mvp, 1, GL_FALSE, glm::value_ptr(pretransform * transform));
		}
	}
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[IBO]);
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_SHORT, 0);
}

void Mesh::CleanUp()
{
	if (!m_loaded) return;
	glDeleteBuffers(NumBuffers, m_buffers);
	glDeleteVertexArrays(1, &m_vao);
	if (m_bones) {
		delete m_bones; m_bones = NULL;
	}
	m_loaded = false;
}


void Mesh::CreateSphere(Mesh * mesh, float radius, unsigned int rings, unsigned int sectors)
{
	const float R = 1.0f / (float)(rings - 1);
	const float S = 1.0f / (float)(sectors - 1);
	unsigned int r, s;

	std::vector<Vertex> vertices;
	vertices.resize(rings*sectors * 3);

	int i = 0;
	for (r = 0; r < rings; ++r)
	for (s = 0; s < sectors; ++s) {
#define M_PI (3.14159265359f)
#define M_PI_2 (1.57079632679f)
		float const y = -sin(-M_PI_2 + M_PI * r * R);
		float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
		float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

		vertices[i].texcoords.x = s*S;
		vertices[i].texcoords.y = r*R;

		vertices[i].position.x = x * radius;
		vertices[i].position.y = y * radius;
		vertices[i].position.z = z * radius;

		vertices[i].normal.x = x;
		vertices[i].normal.y = y;
		vertices[i].normal.z = z;
		++i;
	}

	std::vector<unsigned short> indices;
	indices.resize(rings * sectors * 6);
	std::vector<unsigned short>::iterator id = indices.begin();

	for (r = 0; r < rings; ++r)
	for (s = 0; s < sectors; ++s) {
		*id++ = r * sectors + s;
		*id++ = r * sectors + (s + 1);
		*id++ = (r + 1) * sectors + (s + 1);
		*id++ = r * sectors + s;
		*id++ = (r + 1) * sectors + (s + 1);
		*id++ = (r + 1) * sectors + s;
	}

	mesh->LoadData(vertices, indices);
}


void Mesh::CreateBox(Mesh * mesh, glm::vec3 extents)
{
	using namespace glm;

	float halfSizeX = extents[0];// / 2.0f;
	float halfSizeY = extents[1];// / 2.0f;
	float halfSizeZ = extents[2];// / 2.0f;
	std::vector<Vertex> vertices = {
		// front
		{ vec3(-halfSizeX, -halfSizeY, halfSizeZ), vec3(0.0, 0.0, 1.0), vec2(0.0, 0.0) },
		{ vec3(halfSizeX, -halfSizeY, halfSizeZ), vec3(0.0, 0.0, 1.0), vec2(1.0, 0.0) },
		{ vec3(halfSizeX, halfSizeY, halfSizeZ), vec3(0.0, 0.0, 1.0), vec2(1.0, 1.0) },
		{ vec3(-halfSizeX, halfSizeY, halfSizeZ), vec3(0.0, 0.0, 1.0), vec2(0.0, 1.0) },
		// top
		{ vec3(-halfSizeX, halfSizeY, halfSizeZ), vec3(0.0, 1.0, 0.0), vec2(0.0, 0.0) },
		{ vec3(halfSizeX, halfSizeY, halfSizeZ), vec3(0.0, 1.0, 0.0), vec2(1.0, 0.0) },
		{ vec3(halfSizeX, halfSizeY, -halfSizeZ), vec3(0.0, 1.0, 0.0), vec2(1.0, 1.0) },
		{ vec3(-halfSizeX, halfSizeY, -halfSizeZ), vec3(0.0, 1.0, 0.0), vec2(0.0, 1.0) },
		// back
		{ vec3(halfSizeX, -halfSizeY, -halfSizeZ), vec3(0.0, 0.0, -1.0), vec2(0.0, 0.0) },
		{ vec3(-halfSizeX, -halfSizeY, -halfSizeZ), vec3(0.0, 0.0, -1.0), vec2(1.0, 0.0) },
		{ vec3(-halfSizeX, halfSizeY, -halfSizeZ), vec3(0.0, 0.0, -1.0), vec2(1.0, 1.0) },
		{ vec3(halfSizeX, halfSizeY, -halfSizeZ), vec3(0.0, 0.0, -1.0), vec2(0.0, 1.0) },
		// bottom
		{ vec3(-halfSizeX, -halfSizeY, -halfSizeZ), vec3(0.0, -1.0, 0.0), vec2(0.0, 0.0) },
		{ vec3(halfSizeX, -halfSizeY, -halfSizeZ), vec3(0.0, -1.0, 0.0), vec2(1.0, 0.0) },
		{ vec3(halfSizeX, -halfSizeY, halfSizeZ), vec3(0.0, -1.0, 0.0), vec2(1.0, 1.0) },
		{ vec3(-halfSizeX, -halfSizeY, halfSizeZ), vec3(0.0, -1.0, 0.0), vec2(0.0, 1.0) },
		// left
		{ vec3(-halfSizeX, -halfSizeY, -halfSizeZ), vec3(-1.0, 0.0, 0.0), vec2(0.0, 0.0) },
		{ vec3(-halfSizeX, -halfSizeY, halfSizeZ), vec3(-1.0, 0.0, 0.0), vec2(1.0, 0.0) },
		{ vec3(-halfSizeX, halfSizeY, halfSizeZ), vec3(-1.0, 0.0, 0.0), vec2(1.0, 1.0) },
		{ vec3(-halfSizeX, halfSizeY, -halfSizeZ), vec3(-1.0, 0.0, 0.0), vec2(0.0, 1.0) },
		// right
		{ vec3(halfSizeX, -halfSizeY, halfSizeZ), vec3(1.0, 0.0, 0.0), vec2(0.0, 0.0) },
		{ vec3(halfSizeX, -halfSizeY, -halfSizeZ), vec3(1.0, 0.0, 0.0), vec2(1.0, 0.0) },
		{ vec3(halfSizeX, halfSizeY, -halfSizeZ), vec3(1.0, 0.0, 0.0), vec2(1.0, 1.0) },
		{ vec3(halfSizeX, halfSizeY, halfSizeZ), vec3(1.0, 0.0, 0.0), vec2(0.0, 1.0) },
	};

	std::vector<unsigned short> indices = {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23,
	};
	mesh->LoadData(vertices, indices);
}
