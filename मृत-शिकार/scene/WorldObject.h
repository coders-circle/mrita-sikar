#pragma once

#include "Unit.h"


class WorldObject: public Unit
{
private:
	glm::vec3 m_scale;
	Box m_aabb;
public:
	WorldObject()
	{
		m_liveUnit = false;
		m_tag = 3;	//obstacles
		m_dead = false;
	}
	void UpdateBoundVolume()
	{
		m_boundVolume.parent.SetCenter(m_model->GetBoundVolume().parent.GetCenter() + m_position);
		for (unsigned int i = 0; i < m_boundVolume.children.size(); ++i)
			m_boundVolume.children[i].SetCenter(m_model->GetBoundVolume().children[i].GetCenter() + m_position);

		m_aabb.SetCenter(m_boundVolume.parent.GetCenter());
		m_aabb.SetExtents(glm::vec3(glm::length(m_boundVolume.parent.GetExtents())));
		m_rect = m_aabb.GetRect();

		if (m_scene) m_scene->Reinsert(this);
	}
	void Initialize(Model* m, Scene* s, glm::vec3 pos, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		m_model = m;
		m_position = pos;
		m_scale = scale;
		m_scene = s; 
		m_liveUnit = false;
		m_boundVolume.parent = m_model->GetBoundVolume().parent;
		m_boundVolume.children = m_model->GetBoundVolume().children;
		UpdateBoundVolume();
	}

	void Draw()
	{
		if (m_model)
		{
			m_model->SetTransform(glm::translate(glm::mat4(), m_position)*glm::scale(glm::mat4(), m_scale));
			//m_model->
			m_model->Draw();
		}
	}
};