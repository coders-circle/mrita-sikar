#include "LiveUnit.h"

Scene::Scene(Renderer * renderer) : m_renderer(renderer)
{}

void Scene::Initialize(const Rect &area)
{
	m_quadTree.Initialize(0, area);
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
	m_renderer->BeginRender(Renderer::NORMAL_PASS);

	m_firstPass = true;
	for (unsigned i = 0; i < m_units.size(); ++i)
	if (!m_units[i]->GetDead())
	{
		bool toDraw;
		if (m_units[i]->IsLiveUnit())
			toDraw = m_camera->IntersectBox(static_cast<LiveUnit*>(m_units[i])->GetAABB());
		else
			toDraw = m_camera->IntersectBox(m_units[i]->GetBoundParent());
		if (toDraw)
			m_units[i]->Draw(Renderer::NORMAL_PASS);
	}

	for (unsigned i = 0; i < m_unit2ds.size(); ++i)
	if (!m_unit2ds[i]->GetDead())
		m_unit2ds[i]->Draw();

	m_renderer->EndRender(Renderer::NORMAL_PASS);
}

void Scene::CleanUp()
{
	m_units.clear();
	m_unit2ds.clear();
}