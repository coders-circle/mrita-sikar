#pragma once
#include "../graphics/graphics.h"

class Scene;
class Unit
{
protected:
	Model * m_model;
	Scene * m_scene;

	glm::vec3 m_position;
	bool m_dead;
	int m_tag;			// A number to denote the type of unit (e.g: player: 1, zombie: 2, obstacles: 3)
	bool m_liveUnit;
public:
	Unit();
	Unit(const glm::vec3 &position);
	virtual ~Unit() = 0 {}

	virtual void Initialize(Model * model, glm::vec3 position = glm::vec3()) { m_model = model; m_position = position; }
	virtual void CleanUp() {}

	void SetDead(bool die) { m_dead = die; }
	bool GetDead() const { return m_dead; }

	virtual void Update(double deltaTime);
	virtual void Draw();

	int GetTag() const { return m_tag; }
	void SetScene(Scene * scene) { m_scene = scene; };

	//const BoundVolume& GetBoundVolume() const 
	//{ return m_model->GetBoundVolume(); }

	Box GetBoundParent() const
	{
		return Box(m_model->GetBoundVolume().parent.GetCenter() + m_position, m_model->GetBoundVolume().parent.GetExtents());
	}

	Box GetBoundChild(unsigned int i) const
	{
		return Box(m_model->GetBoundVolume().children[i].GetCenter() + m_position, m_model->GetBoundVolume().children[i].GetExtents());
	}

	bool IsLiveUnit() const { return m_liveUnit; }

};