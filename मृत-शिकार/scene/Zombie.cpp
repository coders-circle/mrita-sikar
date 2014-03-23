#include "Zombie.h"
#include "Scene.h"

#include "../glm/gtx/vector_angle.hpp"
#include "People.h"

enum ZombieStates{
	ZOMBIE_IDLE = 0,
	ZOMBIE_WALK,
	ZOMBIE_FLINCH,
	ZOMBIE_ATTACK1,
	ZOMBIE_ATTACK2,
	ZOMBIE_DEATH,
};

Zombie::Zombie()
{
	Reset();
}

void Zombie::Reset()
{
	m_tag = 2;
	m_attacked = false;
	m_attackunit = NULL;
	m_state = ZOMBIE_IDLE;
	m_ignoreChildren.clear();
	m_ignoreChildren.push_back(3);
	m_ignoreChildren.push_back(4);
	m_orient = glm::mat4();
	m_health = 100;

	m_chaseUnits.clear();

	if (m_model) UpdateBoundVolume();
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

void Zombie::Update(double deltaTime)
{
	bool end = false;
	if (m_state == ZOMBIE_WALK)
		m_model->Advance(m_animation, deltaTime * m_walkspeed, &end);
	else if (m_state == ZOMBIE_ATTACK1 || m_state == ZOMBIE_ATTACK2)
		m_model->Advance(m_animation, deltaTime * m_attackspeed, &end);
	else m_model->Advance(m_animation, deltaTime*1.0f, &end);

	if (end)
	{
		if (m_state == ZOMBIE_FLINCH)
			Idle();
	}

	if (IsIdle()) Walk();

	bool posChanged = false;
	if (m_state != ZOMBIE_DEATH)
	{
		if (IsWalking())
		{
			glm::vec3 resultant(0.0f);

			std::vector<Unit*> units = m_scene->GetUnits();
			/*UnitCollections units;
			m_scene->GetPotentialCollisions(this, units);*/
			for (unsigned int i = 0; i < units.size(); ++i)
			{
				if (units[i] != this)
				{
					if (units[i]->GetTag() == 3 || units[i]->GetTag() == 2)
					{
						glm::vec3  r = this->GetBoundCenter() - units[i]->GetBoundCenter();
						float rlength = glm::length(r);
						float mindist = 500.0f;
						if ((rlength - (this->GetRadius() + units[i]->GetRadius())) < mindist)
						{
							float  U, B, d;
							B = 4200.0f;
							d = rlength / GetRadius();
							U = B / ((units[i]->GetTag() == 2) ? d*d*d : d*d);
							resultant += r / rlength * U;
						}
					}
					else if (units[i]->GetTag() == 1 || units[i]->GetTag() == 10)
					{
						glm::vec3 r = this->GetBoundCenter() - units[i]->GetBoundCenter();
						float distsqr = glm::dot(r, r);

						if (m_chaseUnits.find(units[i]) == m_chaseUnits.end())
						{
							if (static_cast<LiveUnit*>(units[i])->GetHealthStatus() > 0)
							if (CanSee(130.0f, 1000.0f, units[i]))
								m_chaseUnits.insert(units[i]);
						}
						else
						if (distsqr > 25000000.0f || static_cast<LiveUnit*>(units[i])->GetHealthStatus() <= 0)
							m_chaseUnits.erase(units[i]);

						if (distsqr <= 1470.0f)
						if (glm::dot(glm::normalize(-r), glm::vec3(m_orient[2])) > glm::cos(glm::radians(10.0f)))
						{
							Attack();
							m_attackunit = static_cast<LiveUnit*>(units[i]);
						}
					}
				}
			}

			float zmin = -2000, zmax = 2200, xmin = -2000, xmax = 2600;
			glm::vec3 limits[] = { 
				glm::vec3(GetBoundCenter().x, GetBoundCenter().y, zmin),
				glm::vec3(GetBoundCenter().x, GetBoundCenter().y, zmax),
				glm::vec3(xmin, GetBoundCenter().y, GetBoundCenter().z),
				glm::vec3(xmax, GetBoundCenter().y, GetBoundCenter().z), };

			for (int i = 0; i < 4; ++i)
			{
				glm::vec3 r = GetBoundCenter() - limits[i];
				if (glm::dot(r, r) < 160000)
				{
					float  U, B, d;
					B = 10000.0f;
					float rlength = glm::length(r);
					d = rlength / GetRadius();
					U = B / (d*d);
					resultant += r / rlength * U;
				}
			}

			for (std::unordered_set<Unit*>::iterator it = m_chaseUnits.begin(); it != m_chaseUnits.end(); ++it)
			{
				Unit * unit = *it;
				glm::vec3  r = this->GetBoundCenter() - unit->GetBoundCenter();
				float rlength = glm::length(r);
				float  U, A, d;
				A = (unit->GetTag() == 1) ? 4000.0f : 3000.0f;
				d = rlength / GetRadius();
				U = -A / (d*d);
				resultant += r / rlength * U;
			}

			if (resultant.x != 0.0f || resultant.z != 0.0f)
			{
				resultant.y = 0.0f;
				m_orient[2] = glm::vec4(glm::normalize(glm::vec3(m_orient[2]) + resultant*0.006f), 0.0f);
				m_orient[1] = glm::vec4(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
				m_orient[0] = glm::vec4(glm::cross(glm::vec3(m_orient[1]), glm::vec3(m_orient[2])), 0.0f);
				posChanged = true;
			}

			m_position += (glm::vec3)m_orient[2] * (float)deltaTime * 20.0f * m_walkspeed; posChanged = true;
			if (m_a_snoise)
				m_a_snoise->setPosition(irrklang::vec3df(m_position.x, m_position.y, m_position.z));
		}
		else if (IsAttacking())
		{
			if (m_animation.time / m_model->GetAnimationDuration(m_animation.set) >= 0.7f && m_reattack)
			{
				m_attacked = true;
				m_reattack = false;
			}

			if (end) m_reattack = true;

			if (m_attackunit->GetHealthStatus() <= 0
				|| (glm::dot(this->GetBoundCenter() - m_attackunit->GetBoundCenter(), this->GetBoundCenter() - m_attackunit->GetBoundCenter()) > 1470.0f)
				)
				Walk();
		}

		if (posChanged) UpdateBoundVolume();
	}

	
	if (m_state == ZOMBIE_DEATH && !end) return;

	UnitCollections collisions;
	m_scene->GetPotentialCollisions(this, collisions);
	for (unsigned int i = 0; i < collisions.size(); ++i)
	{
		Unit* other = collisions[i];
		if (other != this)
		{
			if (other->IsLiveUnit())
				Collide((LiveUnit*)other);
			else
				Collide(other);
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

	m_boundVolume.parent = m_boundVolume.children[4];
	m_boundVolume.radius = glm::length(m_boundVolume.parent.GetExtents());
	m_aabb.SetCenter(m_boundVolume.parent.GetCenter());
	m_aabb.SetExtents(glm::vec3(m_boundVolume.radius));
	m_rect = m_aabb.GetRect();

	m_ignoreChildren.push_back(0);
	m_ignoreChildren.push_back(1);
	m_ignoreChildren.push_back(2);
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
	m_reattack = true;
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

bool Zombie::IsDead()
{
	return m_state == ZOMBIE_DEATH;
}

bool Zombie::TakeHit(int hitposition, glm::vec3 hitdirection, Unit * player)
{
	if (m_state != ZOMBIE_DEATH && (hitposition == 0 || hitposition == 1 || hitposition == 2))
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

			if (m_chaseUnits.find(player) == m_chaseUnits.end()) m_chaseUnits.insert(player);
		}
		else
		{
			Die();
		}
		return true;
	}
	return false;

}