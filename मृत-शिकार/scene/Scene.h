#pragma once
#include "../graphics/graphics.h"

class Scene
{
protected:
	Renderer * m_renderer;

public:
	Scene(Renderer * renderer = NULL);
	virtual void SetRenderer(Renderer * renderer) { m_renderer = renderer; }
	Renderer* GetRenderer() { return m_renderer; }

	virtual void Resize(float width, float height) {}
	virtual void Update(double deltaTime) {}
	virtual void Draw() {}
	virtual void CleanUp() {}	
};