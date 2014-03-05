#pragma once
#include "Scene.h"

class LiveUnit : public Unit
{
protected:
	glm::vec3 m_velocity;
	Box m_aabb;
	int m_health;

	void UpdateBoundVolume()
	{
		m_boundVolume.parent.SetCenter(glm::mat3(m_orient) * m_model->GetBoundVolume().parent.GetCenter() + m_position);
		for (unsigned int i = 0; i < m_boundVolume.children.size(); ++i)
			m_boundVolume.children[i].SetCenter(glm::mat3(m_orient) * m_model->GetBoundVolume().children[i].GetCenter() + m_position);

		m_aabb.SetCenter(m_boundVolume.parent.GetCenter());
		m_aabb.SetExtents(glm::vec3(glm::length(m_boundVolume.parent.GetExtents())));
		m_rect = m_aabb.GetRect();

		if (m_scene) m_scene->Reinsert(this);
	}

	bool Collide(const Unit * other)
	{
		bool ret = false;
		glm::vec3 out;
		if (GetBoundParent().IntersectBox(glm::mat3(m_orient), other->GetBoundParent(), glm::mat3(), &out))
		{
			m_position += out; UpdateBoundVolume();
		}
		return ret;
	}

	bool Collide(const LiveUnit * other)
	{
		bool ret = false;
		glm::vec3 out;
		if (GetBoundParent().IntersectBox(glm::mat3(m_orient), other->GetBoundParent(), glm::mat3(((LiveUnit*)other)->GetOrient()), &out))
		{
			m_position += out; UpdateBoundVolume(); ret = true;
		}
		return ret;
	}

	bool Collide(const Box &other, const glm::mat3 &otherOrient)
	{
		bool ret = false;
		glm::vec3 out;
		if (GetBoundParent().IntersectBox(glm::mat3(m_orient), other, otherOrient, &out))
		{
			m_position += out; UpdateBoundVolume(); ret = true;
		}
		return ret;
	}

public:
	LiveUnit(const glm::vec3 &position = glm::vec3(0.0f));
	virtual ~LiveUnit() {}

	virtual void Initialize(Model * model, glm::vec3 position = glm::vec3())
	{
		m_model = model; m_position = position;
		m_boundVolume.parent = m_model->GetBoundVolume().parent;
		m_boundVolume.children = m_model->GetBoundVolume().children;
		UpdateBoundVolume();
	}

	virtual void SetVelocity(const glm::vec3& velocity) { m_velocity = velocity; }
	const glm::vec3 &GetVelocity() const { return m_velocity; }

	virtual void SetPosition(const glm::vec3& position) { m_position = position; }
	const glm::vec3 &GetPosition() const { return m_position; }

	virtual void Update() { UpdateBoundVolume(); }
	virtual void Draw()
	{
		if (m_model)
		{
			m_model->SetTransform(glm::translate(glm::mat4(), m_position) * m_orient);
			m_model->Draw();
		}
	}
	virtual void SetRotation(float rotation_angle, glm::vec3 rotation_axes)
	{
		m_orient *= glm::rotate(glm::mat4(), rotation_angle, rotation_axes);
	}

	const Box& GetAABB() const { return m_aabb; }

};