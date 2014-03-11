#include "LiveUnit.h"

Scene::Scene(Renderer * renderer) : m_renderer(renderer), m_skybox(renderer)
{}

void Scene::Initialize(const Rect &area)
{
	m_quadTree.Initialize(0, area);
	m_skybox.Initialize();
}

void Scene::Resize(float width, float height)
{
	if (!m_camera) return;
	m_camera->UpdateProjection(width, height);
	m_renderer->Resize(width, height, m_camera->GetProjection());
}

void Scene::Update(double deltaTime)
{
	if (m_camera) m_camera->UpdateView(deltaTime);

	for (unsigned i = 0; i < m_units.size(); ++i)
		m_units[i]->Update(deltaTime);
}

void Scene::Draw()
{
	if (!m_camera) return;

	m_firstPass = true;
	glm::mat4 camInverse = glm::inverse(m_camera->GetView());
	glm::vec3 target = glm::vec3(camInverse[3] - camInverse[2]);
	m_renderer->SetupLightMatrix(glm::normalize(glm::vec3(-1.0f, -1.0f, 0.0f)), target, 1000, 500, -10000, 100000);
	m_renderer->BeginRender(Renderer::SHADOW_PASS);
	for (unsigned i = 0; i < m_units.size(); ++i)
			m_units[i]->Draw();

	m_firstPass = false;
	m_renderer->UpdateView(m_camera->GetView());
	m_renderer->BeginRender(Renderer::NORMAL_PASS);

	for (unsigned i = 0; i < m_units.size(); ++i)
	{
		bool toDraw;
		if (m_units[i]->IsLiveUnit())
			toDraw = m_camera->IntersectBox(static_cast<LiveUnit*>(m_units[i])->GetAABB());
		else
			toDraw = m_camera->IntersectBox(m_units[i]->GetBoundParent());
		if (toDraw)
			m_units[i]->Draw();
	}

	m_skybox.Draw(glm::vec3(camInverse[3]));
}

void Scene::CleanUp()
{
	m_units.clear();
	m_skybox.CleanUp();
}

bool Scene::CheckPotentialCollision(const Unit * unit1, const Unit * unit2) const
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



Unit* Scene::GetNearestIntersection(const Ray &ray, const Unit * ignoreUnit) const
{
	UnitCollections unitCollections;
	m_quadTree.GetPotentialCollisions(ray, unitCollections);
	Unit * nearestUnit = NULL;
	float tmin = FLT_MAX;
	for (unsigned int i = 0; i < unitCollections.size(); ++i)
	{
		Unit * testUnit = unitCollections[i];
		if (testUnit == ignoreUnit) continue;
		bool test; float t;
		if (testUnit->IsLiveUnit())
			test = ray.IntersectBox(testUnit->GetBoundParent(), static_cast<glm::mat3>(testUnit->GetOrient()), t);
			//test = ray.IntersectBox(((LiveUnit*)testUnit)->GetAABB(), t);
		else
			test = ray.IntersectBox(testUnit->GetBoundParent(), t);

		if (test && t < tmin)
		{
			nearestUnit = testUnit;
			tmin = t;
		}
	}
	return nearestUnit;
}