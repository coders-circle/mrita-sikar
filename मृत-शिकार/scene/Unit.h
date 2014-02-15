#pragma once
#include "../graphics/graphics.h"

class Scene;
class Unit
{
protected:
	Model * m_model;
	Scene * m_scene;

	glm::vec3 m_position;

	bool m_dead;		 // Is dead? Meaning, whether should be updated and drawn by scene or not?
	unsigned char m_tag; // A number to denote the type of unit (e.g: player: 1, zombie: 2, obstacles: 3)
	bool m_liveUnit;	 // Is a live unit?

	BoundVolume m_boundVolume;	//Copy of bound volume, to be updated according to position of unit
	Rect m_rect;


	void UpdateBoundVolume()
	{
		m_boundVolume.parent.SetCenter(m_model->GetBoundVolume().parent.GetCenter() + m_position);
		for (unsigned int i = 0; i < m_boundVolume.children.size(); ++i)
			m_boundVolume.children[i].SetCenter(m_model->GetBoundVolume().children[i].GetCenter() + m_position);
		m_rect = m_boundVolume.parent.GetRect();
	}

public:
	Unit(const glm::vec3 &position = glm::vec3(0.0f));
	//Unit();
	virtual ~Unit() {}

	virtual void Initialize(Model * model, glm::vec3 position = glm::vec3())
	{
		m_model = model; m_position = position;
		m_boundVolume.parent = m_model->GetBoundVolume().parent;
		m_boundVolume.children = m_model->GetBoundVolume().children;
		UpdateBoundVolume();
	}
	virtual void CleanUp() {}

	void SetDead(bool die) { m_dead = die; }
	bool GetDead() const { return m_dead; }

	virtual void Update(double deltaTime) {}
	virtual void Draw()
	{
		if (m_model)
		{
			m_model->SetTransform(glm::translate(glm::mat4(), m_position));
			m_model->Draw();
		}
	}

	int GetTag() const { return m_tag; }
	void SetScene(Scene * scene) { m_scene = scene; };

	const Box &GetBoundParent() const
	{
		return m_boundVolume.parent;
		//return Box(m_model->GetBoundVolume().parent.GetCenter() + m_position, m_model->GetBoundVolume().parent.GetExtents());
	}

	const Box &GetBoundChild(unsigned int i) const
	{
		return m_boundVolume.children[i];
		//return Box(m_model->GetBoundVolume().children[i].GetCenter() + m_position, m_model->GetBoundVolume().children[i].GetExtents());
	}

	const Rect &GetRect() const
	{
		return m_rect;
	}

	bool IsLiveUnit() const { return m_liveUnit; }


};