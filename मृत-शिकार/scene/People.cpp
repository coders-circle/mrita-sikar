#include "People.h"

People::People() : m_once(true)
{
	Reset();
	m_tag = 10;
}

void People::Reset()
{
	m_dead = false;
	m_deadrotation = 0.0;
	m_health = 100;
	m_oncedead = false;
	m_orient = glm::mat4();
}

void People::Update(double deltaTime)
{
	if (m_oncedead) return;
	if (m_once) {
		m_once = false;
		m_model->Advance(m_animation, 0.1);
	}
	if (m_dead)
	{
		m_deadrotation += float(deltaTime) * 105.0f;
		if (m_deadrotation > 85.0f) { m_oncedead = true;  m_orient = glm::rotate(glm::mat4(), 85.0f, glm::vec3(-m_orient[0])); UpdateBoundVolume(); return; }
		m_orient = glm::rotate(glm::mat4(), m_deadrotation, glm::vec3(-m_orient[0]));
		UpdateBoundVolume();
	}
}

void People::Draw()
{
	m_model->SetTransform(glm::translate(glm::mat4(), m_position + glm::vec3(0.0f, m_deadrotation/75, 0.0f)) * m_orient);
	m_model->Animate(m_animation);
	m_model->Draw();
}

void People::Die()
{
	m_health = 0;
	m_dead = true;
}
