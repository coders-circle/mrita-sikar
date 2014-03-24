#pragma once

#include "Unit.h"
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
	void AddUnit(Billboard * billboard) { m_billboards.push_back(billboard); }
	void AddUnit(Unit2d * unit2d) { Scene::AddUnit(unit2d); }

	const std::vector<Unit*> &GetUnits() const { return m_units; }
	const std::vector<Billboard*> &GetBillboards() const { return m_billboards; }

	Unit* GetNearestIntersection(const Ray &ray, float * tmin = NULL, const Unit * ignoreUnit = NULL) const;
	Unit* GetNearestIntersection(const Ray &ray, int &child, float &tmin, const Unit * ignoreUnit = NULL) const;
	bool CheckPotentialCollision(const Unit * unit1, const Unit * unit2) const;
	void GetPotentialCollisions(Unit * unit, UnitCollections &unitCollections) const
	{ m_quadTree.GetPotentialCollisions(unit, unitCollections); }

	void Reset()
	{
		m_texts.clear();
		m_units.clear();
		m_unit2ds.clear();
		m_billboards.clear();
		m_quadTree.Reset();
	}
};

