#include "Zombie.h"

enum ZombieStates{
	ZOMBIE_IDLE = 0,
	ZOMBIE_WALK,
	ZOMBIE_FLINCH,
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
	bool end = false;
	m_model->Advance(m_animation, deltaTime, &end);

	if (end)
	{
		// To do anything when animation reached the end, do it here
	}
}

void Zombie::Draw()
{
	m_model->SetTransform(glm::translate(glm::mat4(), m_position)  * glm::mat4(m_orient));
	m_model->Animate(m_animation);		// use Animate(m_animation, false) if you don't want to loop the animation	
	m_model->Draw();
}

void Zombie::Walk()
{
	m_model->Transition(m_animation, ZOMBIE_WALK, 0.08);
}

void Zombie::Idle()
{
	m_model->Transition(m_animation, ZOMBIE_IDLE, 0.08);
}

void Zombie::Attack()
{

	m_model->Transition(m_animation, ZOMBIE_ATTACK1, 0.08);
}