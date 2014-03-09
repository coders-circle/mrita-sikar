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
	void AddUnit(Unit2d * unit) { m_unit2ds.push_back(unit); }//unit->SetScene(this); }


	const std::vector<Unit*> &GetUnits() const { return m_units; }
	const std::vector<Unit2d*> &GetUnit2ds() const { return m_unit2ds; }

	Unit* GetNearestIntersection(const Ray &ray, const Unit * ignoreUnit = NULL) const;

	bool CheckPotentialCollision(const Unit * unit1, const Unit * unit2) const;

	void GetPotentialCollisions(Unit * unit, UnitCollections &unitCollections) const
	{
		m_quadTree.GetPotentialCollisions(unit, unitCollections);
	}

	/*Ray GeneratePickRay(float mouseX, float mouseY, float screenWidth, float screenHeight) const
	{
		/*glm::vec4 lRayStart_NDC(
			(mouseX / screenWidth - 0.5f) * 2.0f,
			(0.5f - mouseY / screenHeight) * 2.0f,
			-1.0,
			1.0f
			);
		glm::vec4 lRayEnd_NDC(
			(mouseX / screenWidth - 0.5f) * 2.0f,
			((screenHeight - mouseY) / screenHeight - 0.5f) * 2.0f,
			0.0,
			1.0f
			);

		glm::mat4 M = glm::inverse(m_renderer->GetViewProjection3d());
		glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world /= lRayStart_world.w;
		glm::vec4 lRayEnd_world = M * lRayEnd_NDC; lRayEnd_world /= lRayEnd_world.w;

		glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
		lRayDir_world = glm::normalize(lRayDir_world);

		return Ray(glm::vec3(lRayStart_world), lRayDir_world);*/
		//return Ray(glm::vec3(glm::inverse(m_camera->GetView())[3]), -glm::vec3(glm::inverse(m_camera->GetView())[2]));

		/*float normalised_x = 2 * mouseX / screenWidth - 1.0f;
		float normalised_y = 1.0f - 2 * mouseY / screenHeight;

		glm::vec4 near_pointW = glm::inverse(m_renderer->GetViewProjection3d()) * glm::vec4(normalised_x, normalised_y, 0.0f, 1.0f);
		glm::vec3 near_point = glm::vec3(near_pointW / near_pointW.w);
		glm::vec3 camera_pos = glm::vec3(glm::inverse(m_camera->GetView())[3]);
		return Ray(camera_pos, glm::normalize(near_point-camera_pos));*/
	//}
};