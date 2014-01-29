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