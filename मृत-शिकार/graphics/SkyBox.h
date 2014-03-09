#pragma once
#include "Renderer.h"

class SkyBox
{
private:
	enum Buffer_IDs { VBO, IBO, NumBuffers };

	unsigned int m_vao, m_buffers[NumBuffers], m_texture;
	bool m_loaded;
	Renderer * m_renderer;
public:
	SkyBox(Renderer * renderer = NULL);
	void SetRenderer(Renderer * renderer) { m_renderer = renderer; }
	void Draw(const glm::vec3 &center);

	void Initialize();
	void CleanUp();
};

