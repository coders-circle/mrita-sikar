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


	const std::vector<Unit*> &GetUnits() { return m_units; }
	const std::vector<Unit2d*> &GetUnit2ds() { return m_unit2ds; }

	const Unit* GetNearestIntersection(const Ray &ray)
	{
		UnitCollections unitCollections;
		m_quadTree.GetPotentialCollisions(ray, unitCollections);
		const Unit * nearestUnit = NULL;
		float tmin = FLT_MAX;
		for (unsigned int i = 0; i < unitCollections.size(); ++i)
		{
			const Unit * testUnit = unitCollections[i];
			bool test; float t;
			if (testUnit->IsLiveUnit())			
				test = ray.IntersectBox(testUnit->GetBoundParent(), static_cast<glm::mat3>(testUnit->GetOrient()), t);
			else
				test = ray.IntersectBox(testUnit->GetBoundParent(), t);

			if (t < tmin)
			{
				nearestUnit = testUnit;
				tmin = t;
			}
		}
		return nearestUnit;
	}

	bool CheckPotentialCollision(const Unit * unit1, const Unit * unit2)
	{
		const Box &bx1 = unit1->GetBoundParent();
		const Box &bx2 = unit2->GetBoundParent();
		if (unit1->IsLiveUnit())
		{
			const Box &bx11 = static_cast<const LiveUnit*>(unit1)->GetAABB();
			if (unit2->IsLiveUnit())
				return bx11.IntersectBox(static_cast<const LiveUnit*>(unit2)->GetAABB());
			else
				return bx11.IntersectBox(bx2);
		}
		else
		{
			if (unit2->IsLiveUnit())
				return bx1.IntersectBox(static_cast<const LiveUnit*>(unit2)->GetAABB());
			else
				return bx1.IntersectBox(bx2);
		}
	}

	void GetPotentialCollisions(const Unit * unit, UnitCollections &unitCollections)
	{
		m_quadTree.GetPotentialCollisions(unit, unitCollections);
	}
};