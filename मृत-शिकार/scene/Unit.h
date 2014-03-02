#pragma once
#include "../graphics/graphics.h"

class Scene;
class Unit
{
protected:
	Model * m_model;
	glm::mat4 m_orient;
	Scene * m_scene;

	glm::vec3 m_position;

	bool m_dead;		 // Is dead? Meaning, whether should be updated and drawn by scene or not?
	unsigned char m_tag; // A number to denote the type of unit (e.g: player: 1, zombie: 2, obstacles: 3, Ground: 4)
	bool m_liveUnit;	 // Is a live unit?

	BoundVolume m_boundVolume;	//Copy of bound volume, to be updated according to position of unit
	Rect m_rect;


	void UpdateBoundVolume()
	{
		m_boundVolume.parent.SetCenter(glm::mat3(m_orient) * m_model->GetBoundVolume().parent.GetCenter() + m_position);
		m_boundVolume.parent.SetExtents(glm::abs(glm::mat3(m_orient) *m_model->GetBoundVolume().parent.GetExtents()));
		for (unsigned int i = 0; i < m_boundVolume.children.size(); ++i)
		{
			m_boundVolume.children[i].SetCenter(glm::mat3(m_orient) *m_model->GetBoundVolume().children[i].GetCenter() + m_position);
			m_boundVolume.children[i].SetExtents(glm::abs(glm::mat3(m_orient) *m_model->GetBoundVolume().children[i].GetExtents()));
		}
		m_rect = m_boundVolume.parent.GetRect();

	}

public:
	Unit(const glm::vec3 &position = glm::vec3(0.0f));
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
	virtual void Draw(unsigned int pass)
	{
		if (m_model)
		{
			m_model->SetTransform(glm::translate(glm::mat4(), m_position) * m_orient);
			m_model->Draw(pass);
		}
	}

	int GetTag() const { return m_tag; }
	void SetScene(Scene * scene) { m_scene = scene; };

	const Box &GetBoundParent() const
	{
		return m_boundVolume.parent;
	}

	const Box &GetBoundChild(unsigned int i) const
	{
		return m_boundVolume.children[i];
	}

	const Rect &GetRect() const
	{
		return m_rect;
	}

	bool IsLiveUnit() const { return m_liveUnit; }
	
	glm::vec3 GetPosition() const
	{
		return m_position;
	}

	void RotateX(float xangle) { m_orient = glm::rotate(glm::mat4(), xangle, glm::vec3(1.0f, 0.0f, 0.0f)) * m_orient; UpdateBoundVolume(); }
	void RotateY(float yangle) { m_orient = glm::rotate(glm::mat4(), yangle, glm::vec3(0.0f, 1.0f, 0.0f)) * m_orient; UpdateBoundVolume(); }
	void RotateZ(float zangle) { m_orient = glm::rotate(glm::mat4(), zangle, glm::vec3(0.0f, 0.0f, 1.0f)) * m_orient; UpdateBoundVolume(); }

	glm::vec3 GetBoundCenter(){ return m_boundVolume.parent.GetCenter(); }
	glm::vec3 GetBoundExtents(){ return m_boundVolume.parent.GetExtents(); }

};