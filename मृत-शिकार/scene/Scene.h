#pragma once
#include "Unit.h"
#include "Unit2d.h"
#include "Camera.h"

class Scene
{
private:
	Renderer * m_renderer;
	std::vector<Unit*> m_units;
	std::vector<Unit2d*> m_unit2ds;
	Camera * m_camera;

public:
	Scene(Renderer * renderer = NULL);
	void SetRenderer(Renderer * renderer) { m_renderer = renderer; }
	Renderer* GetRenderer() { return m_renderer; }

	void Initialize();
	void Resize(float width, float height);
	void Update(double deltaTime);
	void Draw();
	void CleanUp();
	
	void SetCamera(Camera * camera) { m_camera = camera; }
	void AddUnit(Unit * unit) { m_units.push_back(unit); unit->SetScene(this); }
	void AddUnit(Unit2d * unit) { m_unit2ds.push_back(unit); }//unit->SetScene(this); }

	bool CheckPotentialCollision(const Unit * unit1, const Unit * unit2);
	const std::vector<Unit*> &GetUnits() { return m_units; }
	const std::vector<Unit2d*> &GetUnit2ds() { return m_unit2ds; }
};