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

	bool CheckPotentialCollision(const Unit * unit1, const Unit * unit2);

	void GetPotentialCollisions(const Unit * unit, UnitCollections &unitCollections)
	{
		m_quadTree.GetPotentialCollisions(unit, unitCollections);
	}

	static Ray GeneratePickRay(float mouseX, float mouseY, float screenWidth, float screenHeight, const glm::mat4 viewProjection)
	{
		// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
		glm::vec4 lRayStart_NDC(
			(mouseX / screenWidth - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
			(mouseY / screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
			-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
			1.0f
			);
		glm::vec4 lRayEnd_NDC(
			(mouseX / screenWidth - 0.5f) * 2.0f,
			(mouseY / screenHeight - 0.5f) * 2.0f,
			0.0,
			1.0f
			);


		glm::mat4 M = glm::inverse(viewProjection);
		glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world /= lRayStart_world.w;
		glm::vec4 lRayEnd_world = M * lRayEnd_NDC; lRayEnd_world /= lRayEnd_world.w;


		glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
		lRayDir_world = glm::normalize(lRayDir_world);

		return Ray(glm::vec3(lRayStart_world), glm::vec3(lRayDir_world));
	}
};