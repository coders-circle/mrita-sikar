#pragma once
#include "../graphics/graphics.h"

class GameScene;
class Unit
{
protected:
	Model * m_model;
	glm::mat4 m_orient;
	GameScene * m_scene;

	glm::vec3 m_position;

	unsigned char m_tag; // A number to denote the type of unit (e.g: player: 1, zombie: 2, obstacles: 3, Ground: 4)
	bool m_liveUnit;	 // Is a live unit?

	BoundVolume m_boundVolume;	// Copy of bound volume, to be updated according to position of unit
	std::vector<unsigned int> m_ignoreChildren;	// List of children-box indices to ignore while testing for collision
	Rect m_rect;

	bool m_visible;

	int GetRand(int range){ return rand() % range; }


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
		m_boundVolume.radius = m_model->GetBoundVolume().radius;
		UpdateBoundVolume();
	}
	virtual void CleanUp() {}

	virtual void Update(double deltaTime) {}
	virtual void Draw()
	{
		if (m_model)
		{
			m_model->SetTransform(glm::translate(glm::mat4(), m_position) * m_orient);
			m_model->Draw();
		}
	}

	int GetTag() const { return m_tag; }
	void SetScene(GameScene * scene) { m_scene = scene; };

	const Box &GetBoundParent() const { return m_boundVolume.parent; }
	const Box &GetBoundChild(unsigned int i) const { return m_boundVolume.children[i]; }
	unsigned int GetChildrenSize() const { return m_boundVolume.children.size(); }
	const std::vector<unsigned int> & GetIgnoreChildren() const { return m_ignoreChildren; }
	const Rect &GetRect() const	{ return m_rect; }

	const glm::mat4 &GetOrient() const { return m_orient; }
	const glm::vec3 &GetPosition() const { return m_position; }

	bool IsLiveUnit() const { return m_liveUnit; }

	void RotateX(float xangle) { m_orient = glm::rotate(glm::mat4(), xangle, glm::vec3(1.0f, 0.0f, 0.0f)) * m_orient; UpdateBoundVolume(); }
	void RotateY(float yangle) { m_orient = glm::rotate(glm::mat4(), yangle, glm::vec3(0.0f, 1.0f, 0.0f)) * m_orient; UpdateBoundVolume(); }
	void RotateZ(float zangle) { m_orient = glm::rotate(glm::mat4(), zangle, glm::vec3(0.0f, 0.0f, 1.0f)) * m_orient; UpdateBoundVolume(); }

	glm::vec3 GetBoundCenter() const { return m_boundVolume.parent.GetCenter(); }
	glm::vec3 GetBoundExtents() const { return m_boundVolume.parent.GetExtents(); }
	float GetRadius() const { return m_boundVolume.radius; }

	void SetVisible(bool visible) { m_visible = visible; }
	bool GetVisible() { return m_visible; }
};