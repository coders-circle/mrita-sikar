#include "Scene.h"

Scene::Scene(Renderer * renderer) : m_renderer(renderer)
{}

void Scene::Initialize()
{
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
	if (!m_units[i]->GetDead())
		m_units[i]->Update(deltaTime);
	for (unsigned i = 0; i < m_unit2ds.size(); ++i)
	if (!m_unit2ds[i]->GetDead())
		m_unit2ds[i]->Update(deltaTime);
}

void Scene::Draw()
{
	if (!m_camera) return;
	m_renderer->UpdateView(m_camera->GetView());
	m_renderer->BeginRender();

	for (unsigned i = 0; i < m_units.size(); ++i)
	if (!m_units[i]->GetDead())
		m_units[i]->Draw();
	for (unsigned i = 0; i < m_unit2ds.size(); ++i)
	if (!m_unit2ds[i]->GetDead())
		m_unit2ds[i]->Draw();

	m_renderer->EndRender();
}

void Scene::CleanUp()
{
	m_units.clear();
}

bool Scene::CheckPotentialCollision(const Unit * unit1, const Unit * unit2, glm::vec3 * out)
{
	const Box &bx1 = unit1->GetBoundParent();
	const Box &bx2 = unit2->GetBoundParent();
	if (unit1->IsLiveUnit())
	{
		Box bx11(bx1.GetCenter(), glm::vec3(glm::length(bx1.GetExtents())));
		if (unit2->IsLiveUnit())
			return bx11.IntersectBox(Box(bx2.GetCenter(), glm::vec3(glm::length(bx2.GetExtents()))), out);
		else
			return bx11.IntersectBox(bx2, out);
	}
	else
	{
		if (unit2->IsLiveUnit())
			return bx1.IntersectBox(Box(bx2.GetCenter(), glm::vec3(glm::length(bx2.GetExtents()))), out);
		else
			return bx1.IntersectBox(bx2, out);
	}
}