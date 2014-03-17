#pragma once
#include "GameScene.h"

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
			if (other->GetChildrenSize() > 0)
			{
				for (unsigned i = 0; i < other->GetChildrenSize(); ++i)
				if (GetBoundParent().IntersectBox(glm::mat3(m_orient), other->GetBoundChild(i), glm::mat3(), &out))
				{
					m_position += out;
					UpdateBoundVolume(); ret = true;
				}
			}
			else
			{
				m_position += out;
				UpdateBoundVolume(); ret = true;
			}
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
	virtual int GetHealthStatus(){ return m_health; }
	const Box& GetAABB() const { return m_aabb; }

	bool IsInView(float fovAngle, float maxdistance, Unit * unit)
	{
		glm::vec3 v1 = glm::normalize(unit->GetPosition() - m_position);
		glm::vec3 v2 = (glm::vec3)m_orient[2];
		if (glm::dot(v1, v1) <= maxdistance)
		if (glm::dot(v1, v2) >= glm::cos(glm::radians(fovAngle)))
			return true;
		return false;
	}
	bool CanSee(float fovAngle, float maxdistance, Unit * unit, GameScene* scene)
	{
		if (!IsInView(fovAngle, maxdistance, unit)) return false;
		Ray ray(glm::vec3(m_position.x, 0.0f, m_position.z), (glm::vec3)m_orient[2]);
		float tmin; int position;
		if (Unit * testunit = scene->GetNearestIntersection(ray, position, tmin, this))
		if (testunit != unit) return false;
		
		return true;
	}

};