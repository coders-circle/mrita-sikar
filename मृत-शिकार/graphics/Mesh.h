#pragma once
#include "Animation.h"
#include "Renderer.h"

struct Vertex
{
	glm::vec3 position, normal;
	glm::vec2 texcoords;
};

struct SkinVertex
{
	glm::vec3 position, normal;
	glm::vec2 texcoords;
	WeightInfo weights;
};

class Mesh
{
private:
	enum Buffer_IDs { VBO, IBO, NumBuffers };

	unsigned int m_vao, m_buffers[NumBuffers], m_texture;
	std::vector<Bone> * m_bones;
	unsigned int m_numIndices;
	bool m_loaded;
	Renderer * m_renderer;
public:
	Mesh(Renderer * renderer = NULL);
	void SetRenderer(Renderer * renderer) { m_renderer = renderer; }

	void LoadData(const std::vector<Vertex> &vertices, const std::vector<unsigned short> &indices);
	void LoadData(const std::vector<SkinVertex> &vertices, const std::vector<unsigned short> &indices);
	void Draw(const glm::mat4 &transform);
	void CleanUp();

	void SetTexture(unsigned int texture) { m_texture = texture; }
	unsigned int GetTexture() { return m_texture; }

	static void CreateBox(Mesh* mesh, glm::vec3 extents);
	static void CreateSphere(Mesh* mesh, float radius, unsigned int rings, unsigned int sectors);

	std::vector<Bone>* &GetBones() { return m_bones; }
};

