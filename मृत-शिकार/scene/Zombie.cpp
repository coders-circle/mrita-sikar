#include "Zombie.h"

enum ZombieStates{
	ZOMBIE_IDLE,
	ZOMBIE_WALK,
	ZOMBIE_FLICH,
	ZOMBIE_ATTACK1,
	ZOMBIE_ATTACK2,
	ZOMBIE_DEATH,
};

Zombie::Zombie() : m_state(ZOMBIE_IDLE)
{
	m_tag = 2;
}

void Zombie::Update(double deltaTime)
{
	m_model->Advance(m_animation, deltaTime);
}

void Zombie::Draw()
{
	m_model->SetTransform(glm::translate(glm::mat4(), m_position)  * glm::mat4(m_orient));
	m_model->Animate(m_animation);
	m_model->Draw();
}