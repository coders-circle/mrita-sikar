#pragma once
#include "Unit.h"

class LiveUnit : public Unit
{
protected:
	glm::vec3 m_velocity;
	glm::mat4 m_orient;	

	void UpdateBoundVolume()
	{
		m_boundVolume.parent.SetCenter(m_model->GetBoundVolume().parent.GetCenter() * glm::mat3(m_orient) + m_position);
		for (unsigned int i = 0; i < m_boundVolume.children.size(); ++i)
			m_boundVolume.children[i].SetCenter(m_model->GetBoundVolume().children[i].GetCenter() * glm::mat3(m_orient) + m_position);
	}
public:
	LiveUnit(const glm::vec3 &position = glm::vec3(0.0f));
	virtual ~LiveUnit() {}

	virtual void SetVelocity(const glm::vec3& velocity) { m_velocity = velocity; }
	const glm::vec3 &GetVelocity() const { return m_velocity; }

	virtual void SetPosition(const glm::vec3& position) { m_position = position; }
	const glm::vec3 &GetPosition() const { return m_position; }

	const glm::mat4 &GetOrient() const { return m_orient; }

	virtual void Update() { UpdateBoundVolume(); }
	virtual void Draw()
	{
		if (m_model)
		{
			m_model->SetTransform(glm::translate(glm::mat4(), m_position) * m_orient);
			m_model->Draw();
		}
	}

};