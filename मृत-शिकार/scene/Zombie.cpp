#include "Zombie.h"
#include "Scene.h"

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

irrklang::ISoundSource* g_a_noise;

void InitNoise()
{
	static bool initialized = false;
	if (!initialized)
	{
		g_a_noise = g_audioengine->addSoundSourceFromFile("sound/zombie/zombie_noise.mp3", irrklang::ESM_AUTO_DETECT, true);
		initialized = true;
	}
}

void Zombie::InitAudio()
{
	InitNoise();
	m_a_snoise = g_audioengine->play3D(g_a_noise, irrklang::vec3df(m_position.x, m_position.y, m_position.z), true, false, true);
	if (m_a_snoise)
	{

		m_a_snoise->setMinDistance(50.0f);
		m_a_snoise->setMaxDistance(2000.0f);
	}
}
void Zombie::Update(double deltaTime)
{
	bool end = false;
	m_model->Advance(m_animation, deltaTime * 1.5, &end);

	if (end)
	{
		// To do anything when animation reached the end, do it here
	}

	bool posChanged = false;
	switch (m_state)
	{
	case ZOMBIE_WALK:
		m_position += (glm::vec3)m_orient[2] * (float)deltaTime * 20.0f *2.0f; posChanged = true;
		if (m_a_snoise)
		{
			m_a_snoise->setPosition(irrklang::vec3df(m_position.x, m_position.y, m_position.z));
		}
	}

	if (posChanged) UpdateBoundVolume();

	UnitCollections collisions;
	m_scene->GetPotentialCollisions(this, collisions);
	for (unsigned int i = 0; i < collisions.size(); ++i)
	{
		const Unit* other = collisions[i];
		if (other != this)
		{
			if (other->IsLiveUnit())
				Collide((LiveUnit*)other);
			else
				Collide(other);
		}
	}
}

void Zombie::Draw(unsigned int pass)
{
	m_model->SetTransform(glm::translate(glm::mat4(), m_position)  * m_orient);
	if (m_scene->IsFirstPass()) m_model->Animate(m_animation);
	m_model->Draw(pass);
}

void Zombie::Walk()
{
	m_model->Transition(m_animation, ZOMBIE_WALK, 1.00);
	m_state = ZOMBIE_WALK;
}

void Zombie::Idle()
{
	m_model->Transition(m_animation, ZOMBIE_IDLE, 0.08);
	m_state = ZOMBIE_IDLE;
}

void Zombie::Flinch()
{
	m_model->Transition(m_animation, ZOMBIE_FLINCH, 0.08);
	m_state = ZOMBIE_FLINCH;
}

void Zombie::Attack()
{
	m_model->Transition(m_animation, ZOMBIE_ATTACK1, 0.08);
	m_state = ZOMBIE_ATTACK1;
}

bool Zombie::IsAttacking()
{
	return m_state == ZOMBIE_ATTACK1;
}

bool Zombie::IsIdle()
{
	return m_state == ZOMBIE_IDLE;
}

bool Zombie::IsWalking()
{
	return m_state == ZOMBIE_WALK;
}