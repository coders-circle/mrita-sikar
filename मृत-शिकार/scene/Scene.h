#pragma once
#include "Unit.h"
#include "Unit2d.h"
#include "Camera.h"
#include "../collision/QuadTree.h"

class Scene
{
private:
	Renderer * m_renderer;
	std::vector<Unit*> m_units;
	std::vector<Unit2d*> m_unit2ds;
	Camera * m_camera;

	QuadTree m_quadTree;
	bool m_firstPass;

public:
	Scene(Renderer * renderer = NULL);
	void SetRenderer(Renderer * renderer) { m_renderer = renderer; }
	Renderer* GetRenderer() { return m_renderer; }

	bool IsFirstPass() { return m_firstPass; }

	void Initialize(const Rect &area);
	void Resize(float width, float height);
	void Update(double deltaTime);
	void Draw();
	void CleanUp();
	
	void SetCamera(Camera * camera) { m_camera = camera; }
	void AddUnit(Unit * unit) { m_units.push_back(unit); unit->SetScene(this);  m_quadTree.Insert(unit); }
	void Reinsert(Unit * unit) { m_quadTree.Remove(unit); m_quadTree.Insert(unit); }
	void AddUnit(Unit2d * unit) { m_unit2ds.push_back(unit); }//unit->SetScene(this); }

	bool CheckPotentialCollision(const Unit * unit1, const Unit * unit2);
	void GetPotentialCollisions(const Unit * unit, UnitCollections &unitCollections) 
	{ m_quadTree.GetPotentialCollisions(unit, unitCollections); }

	const std::vector<Unit*> &GetUnits() { return m_units; }
	const std::vector<Unit2d*> &GetUnit2ds() { return m_unit2ds; }
};