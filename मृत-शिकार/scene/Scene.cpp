#include "LiveUnit.h"

Scene::Scene(Renderer * renderer) : m_renderer(renderer)
{}

void Scene::Initialize()
{
	m_quadTree.Initialize(0, Rect(-5000, -5000, 10000, 10000));
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
	{
		bool toDraw;
		if (m_units[i]->IsLiveUnit())
			toDraw = m_camera->GetBoundBox().IntersectBox(static_cast<LiveUnit*>(m_units[i])->GetAABB());
		else
			toDraw = m_camera->GetBoundBox().IntersectBox(m_units[i]->GetBoundParent());
		if (toDraw)
			m_units[i]->Draw();
	}

	for (unsigned i = 0; i < m_unit2ds.size(); ++i)
	if (!m_unit2ds[i]->GetDead())
		m_unit2ds[i]->Draw();

	m_renderer->EndRender();
}

void Scene::CleanUp()
{
	m_units.clear();
}

bool Scene::CheckPotentialCollision(const Unit * unit1, const Unit * unit2)
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
	/*UnitCollections coll;
	m_quadTree.GetPotentialCollisions(unit1, coll);
	for (unsigned int i = 0; i < coll.size(); ++i)
	for (unsigned int j = 0; j < coll[i][0].size(); ++j)
	if (unit2 == coll[i][0][j]) return true;
	return false;*/
}