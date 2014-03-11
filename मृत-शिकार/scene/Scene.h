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
	Camera * m_camera;

	QuadTree m_quadTree;
	bool m_firstPass;

	SkyBox m_skybox;

public:
	Scene(Renderer * renderer = NULL);
	void SetRenderer(Renderer * renderer) { m_renderer = renderer; m_skybox.SetRenderer(m_renderer); }
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


	const std::vector<Unit*> &GetUnits() const { return m_units; }

	Unit* GetNearestIntersection(const Ray &ray, const Unit * ignoreUnit = NULL) const;

	bool CheckPotentialCollision(const Unit * unit1, const Unit * unit2) const;

	void GetPotentialCollisions(Unit * unit, UnitCollections &unitCollections) const
	{
		m_quadTree.GetPotentialCollisions(unit, unitCollections);
	}
};