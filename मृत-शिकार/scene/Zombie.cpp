#include "Zombie.h"
#include "Scene.h"

#include "../glm/gtx/vector_angle.hpp"

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
	m_avoidingObstacle = false;
	m_isstruck = false;
	m_attacked = false;
}

irrklang::ISoundSource* g_a_noise;
irrklang::ISoundSource* g_a_zombiepain1;
irrklang::ISoundSource* g_a_zombiepain2;
irrklang::ISoundSource* g_a_zombiepain3;
irrklang::ISoundSource* g_a_zombiepain4;
irrklang::ISoundSource* g_a_zombiepain5;
irrklang::ISoundSource* g_a_zombiemadness1;
irrklang::ISoundSource* g_a_zombienoise;

void InitZombieSounds()
{
	static bool initialized = false;
	if (!initialized)
	{
		g_a_zombienoise = g_audioengine->addSoundSourceFromFile("sound/zombie/zombie_noise.mp3", irrklang::ESM_AUTO_DETECT, true);
		g_a_zombiemadness1 = g_audioengine->addSoundSourceFromFile("sound/zombie/zombie_madness1.mp3", irrklang::ESM_AUTO_DETECT, true);
		g_a_zombiepain1 = g_audioengine->addSoundSourceFromFile("sound/zombie/zombie_pain1.mp3", irrklang::ESM_AUTO_DETECT, true);
		g_a_zombiepain2 = g_audioengine->addSoundSourceFromFile("sound/zombie/zombie_pain2.mp3", irrklang::ESM_AUTO_DETECT, true);
		g_a_zombiepain3 = g_audioengine->addSoundSourceFromFile("sound/zombie/zombie_pain3.mp3", irrklang::ESM_AUTO_DETECT, true);
		g_a_zombiepain4 = g_audioengine->addSoundSourceFromFile("sound/zombie/zombie_pain4.mp3", irrklang::ESM_AUTO_DETECT, true);
		g_a_zombiepain5 = g_audioengine->addSoundSourceFromFile("sound/zombie/zombie_pain5.mp3", irrklang::ESM_AUTO_DETECT, true);
		initialized = true;
	}
}

void Zombie::InitAudio()
{
	InitZombieSounds();
	
}

void Zombie::SetDestination(glm::vec3 destination)
{
	m_destination = destination;
}

void Zombie::Update(double deltaTime)
{
	bool end = false;
	if (m_state == ZOMBIE_WALK)
	{
		m_model->Advance(m_animation, deltaTime * m_walkspeed, &end);
	}
	else if (m_state == ZOMBIE_ATTACK1 || m_state == ZOMBIE_ATTACK2)
	{
		m_model->Advance(m_animation, deltaTime * m_attackspeed, &end);
	}
	else
	m_model->Advance(m_animation, deltaTime*1.0f, &end);

	if (end)
	{
		// To do anything when animation reached the end, do it here
		if (m_state == ZOMBIE_ATTACK1 || m_state == ZOMBIE_ATTACK2)
		{
			m_attacked = true;
		}
		if (m_state == ZOMBIE_FLINCH)
		{
			Idle();
		}

	}


	if (m_state != ZOMBIE_DEATH)
	{
		glm::vec3 dist = m_destination - this->GetBoundCenter();
		float distsq = glm::dot(dist, dist);

		glm::vec3 fa = glm::normalize(dist);
		float fmf = 1.0f;
		fa.x *= fmf;
		fa.y *= fmf;
		fa.z *= fmf;
		glm::vec3 resultant = fa;
		/*if (!m_isstruck)
		{
			resultant = fa;
		}*/


		if (IsWalking())
		{
			std::vector<Unit*> units = m_scene->GetUnits();
			for (unsigned int i = 0; i < units.size(); i++)
			{
				if (units[i] != this)
				{
					if (units[i]->GetTag() == 3 || units[i]->GetTag() == 2)
					{
						glm::vec3 r = (this->GetBoundCenter() - units[i]->GetBoundCenter());
						float d1 = glm::dot(r, r);
						float d2 = glm::dot(units[i]->GetBoundExtents(), units[i]->GetBoundExtents());
						if (m_avoidingObstacle == true && (d2 + 100000.0f > d1) && (d2 + 10000.0f <= d1))
						{
							glm::vec3 f = glm::normalize(r);
							float mf = 55.0f*(float)(glm::sqrt(d2) / d1);
							
							f.x *= mf;
							f.y *= mf;
							f.z *= mf;
							resultant += f;

						}
						else if (d2 + 10000.0f > d1)
						{
							glm::vec3 f = glm::normalize(r);
							float mf = 40.0f*(float)(glm::sqrt(d2) / d1);
							/*if (units[i]->GetTag() == 2)
							{
								mf *= 0.2f;
							}*/
							f.x *= mf;
							f.y *= mf;
							f.z *= mf;
							resultant += f;
							m_avoidingObstacle = true;
						}
						else
						{
							m_avoidingObstacle = false;
						}
					}
				}
			}
		}
		if (resultant.x == 0.0f && resultant.z == 0.0f)
		{

		}
		else
		{
			resultant.y = 0.0f;
			m_orient[2] = glm::vec4(glm::normalize(resultant), 0.0f);
			m_orient[1] = glm::vec4(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
			m_orient[0] = (glm::vec4(glm::normalize(glm::cross(glm::vec3(m_orient[1]), glm::vec3(m_orient[2]))), 0.0f));
		}
		if (distsq > 2500.0f)
		{
			if (this->IsWalking() == false && this->m_state != ZOMBIE_FLINCH) this->Walk();
		}
		else if (this->IsAttacking() == false && this->m_state != ZOMBIE_FLINCH) this->Attack();
	}
	bool posChanged = false;
	switch (m_state)
	{
	case ZOMBIE_WALK:
		m_position += (glm::vec3)m_orient[2] * (float)deltaTime * 20.0f * m_walkspeed; posChanged = true;
		if (m_a_snoise)
		{
			m_a_snoise->setPosition(irrklang::vec3df(m_position.x, m_position.y, m_position.z));
		}
		break;
	}

	if (posChanged) UpdateBoundVolume();

	m_isstruck = false;
	UnitCollections collisions;
	m_scene->GetPotentialCollisions(this, collisions);
	for (unsigned int i = 0; i < collisions.size(); ++i)
	{
		Unit* other = collisions[i];
		if (other != this)
		{
			if (other->IsLiveUnit())
			{
				if (Collide((LiveUnit*)other)) m_isstruck = true;
			}
			else
				if (Collide(other)) m_isstruck = true;
		}
	}

	

}

void Zombie::Draw()
{
	m_model->SetTransform(glm::translate(glm::mat4(), m_position)  * m_orient);
	m_model->Animate(m_animation, m_state!=ZOMBIE_DEATH);
	m_model->Draw();
}

void Zombie::Die()
{
	if (m_a_snoise) m_a_snoise->stop();
	m_model->Transition(m_animation, ZOMBIE_DEATH, 0.08);
	m_state = ZOMBIE_DEATH;
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

void Zombie::TakeHit(int hitposition, glm::vec3 hitdirection)
{
	if (m_state != ZOMBIE_DEATH)
	{
		switch (hitposition)
		{
		case 0:	m_health -= 100; break;
		case 1: m_health -= 50; break;
		case 2: m_health -= 25; break;
		}

		/*m_orient[2] = glm::vec4(glm::normalize(hitdirection - GetBoundCenter()), 0.0f);
		m_orient[1] = glm::vec4(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
		m_orient[0] = glm::normalize(glm::vec4(glm::cross(glm::vec3(m_orient[1]), glm::vec3(m_orient[2])), 0.0f));*/
		m_position += glm::normalize(hitdirection)*20.0f;
		UpdateBoundVolume();

	//	UpdateBoundVolume();

		if (m_health > 0)
		{
			if (g_audioengine->isCurrentlyPlaying(g_a_zombiemadness1) == true)
			{
				m_a_madness->stop();
			}
			switch (GetRand(5))
			{
			case 0: m_a_pain = g_audioengine->play3D(g_a_zombiepain1, irrklang::vec3df(m_position.x, m_position.y, m_position.z), false, false, true); break;
			case 1: m_a_pain = g_audioengine->play3D(g_a_zombiepain2, irrklang::vec3df(m_position.x, m_position.y, m_position.z), false, false, true); break;
			case 2: m_a_pain = g_audioengine->play3D(g_a_zombiepain3, irrklang::vec3df(m_position.x, m_position.y, m_position.z), false, false, true); break;
			case 3: m_a_pain = g_audioengine->play3D(g_a_zombiepain4, irrklang::vec3df(m_position.x, m_position.y, m_position.z), false, false, true); break;
			default: m_a_pain = g_audioengine->play3D(g_a_zombiepain5, irrklang::vec3df(m_position.x, m_position.y, m_position.z), false, false, true);
			}
			if (m_a_pain)
			{
				m_a_pain->setMinDistance(50.0f);
				m_a_pain->setMaxDistance(2000.0f);
			}
			//m_position -= glm::normalize(hitdirection - GetBoundCenter()) * 20.0f;
			Flinch();
		}
		else
		{
			Die();
		}
	}

}