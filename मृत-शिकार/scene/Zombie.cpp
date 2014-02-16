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
	m_a_snoise->setMinDistance(100.0f);
	m_a_snoise->setMaxDistance(2000.0f);
}
void Zombie::Update(double deltaTime)
{
	bool end = false;
	m_model->Advance(m_animation, deltaTime, &end);

	if (end)
	{
		// To do anything when animation reached the end, do it here
	}

	bool posChanged = false;
	switch (m_state)
	{
	case ZOMBIE_WALK:
		m_position += (glm::vec3)m_orient[2] * (float)deltaTime * 20.0f; posChanged = true;
		if (m_a_snoise)
		{
			m_a_snoise->setPosition(irrklang::vec3df(m_position.x, m_position.y, m_position.z));
		}
	}

	if (posChanged) UpdateBoundVolume();

	UnitCollections collisions;
	m_scene->GetPotentialCollisions(this, collisions);
	for (unsigned int i = 0; i < collisions.size(); ++i)
	for (UnitIterator j = collisions[i]->begin(); j != collisions[i]->end(); ++j)
	{
		const Unit* other = *j;
		if (other->GetTag() == 1)
		if (m_scene->CheckPotentialCollision(this, other))
		{
			glm::vec3 out;
			if (GetBoundParent().IntersectBox((glm::mat3)m_orient, other->GetBoundParent(), (glm::mat3)((LiveUnit*)other)->GetOrient(), &out))
			{
				m_position += out; UpdateBoundVolume();
			}
			if (IsAttacking())
			if (GetBoundChild(3).IntersectBox((glm::mat3)m_orient, other->GetBoundParent(), (glm::mat3)((LiveUnit*)other)->GetOrient(), &out))
			{
				m_position += out; UpdateBoundVolume();
			}
		}
	}
}

void Zombie::Draw()
{
	m_model->SetTransform(glm::translate(glm::mat4(), m_position)  * m_orient);
	m_model->Animate(m_animation);		// use Animate(m_animation, false) if you don't want to loop the animation	
	m_model->Draw();
}

void Zombie::Walk()
{
	m_model->Transition(m_animation, ZOMBIE_WALK, 0.08);
	m_state = ZOMBIE_WALK;
}

void Zombie::Idle()
{
	m_model->Transition(m_animation, ZOMBIE_IDLE, 0.08);
	m_state = ZOMBIE_IDLE;
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