#pragma once

#include "Unit.h"
#include "Unit2d.h"
#include "Billboard.h"
#include "Camera.h"
#include "../collision/QuadTree.h"
#include "Scene.h"

class GameScene : public Scene
{
private:
	QuadTree m_quadTree;
	bool m_firstPass;

	Camera * m_camera;
	std::vector<Unit*> m_units;
	std::vector<Unit2d*> m_unit2ds;
	std::vector<Billboard*> m_billboards;
	SkyBox m_skybox;
public:
	GameScene(Renderer * renderer = NULL); 
	void SetRenderer(Renderer * renderer) { m_renderer = renderer; m_skybox.SetRenderer(m_renderer); }

	void Initialize(const Rect &area);
	void Resize(float width, float height);
	void Update(double deltaTime);
	void Draw();
	void CleanUp();

	bool IsFirstPass() { return m_firstPass; }
	void SetCamera(Camera * camera) { m_camera = camera; }
	void AddUnit(Unit * unit) { m_units.push_back(unit); unit->SetScene(this);  m_quadTree.Insert(unit); }
	void Reinsert(Unit * unit) { m_quadTree.Remove(unit); m_quadTree.Insert(unit); }
	void AddUnit(Unit2d * unit2d) { m_unit2ds.push_back(unit2d); }
	void AddUnit(Billboard * billboard) { m_billboards.push_back(billboard); }

	const std::vector<Unit*> &GetUnits() const { return m_units; }

	Unit* GetNearestIntersection(const Ray &ray, const Unit * ignoreUnit = NULL) const;
	bool CheckPotentialCollision(const Unit * unit1, const Unit * unit2) const;
	void GetPotentialCollisions(Unit * unit, UnitCollections &unitCollections) const
	{ m_quadTree.GetPotentialCollisions(unit, unitCollections); }
};

