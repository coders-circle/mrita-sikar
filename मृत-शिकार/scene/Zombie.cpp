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

void Zombie::SetDestination(glm::vec3 destination)
{
	m_destination = destination;
}

void Zombie::Update(double deltaTime)
{
	bool end = false;
	m_model->Advance(m_animation, deltaTime * 1.5, &end);

	if (end)
	{
		// To do anything when animation reached the end, do it here
	}

	float rotang = glm::angle(glm::vec3(m_orient[2]), glm::normalize(m_destination-this->GetBoundCenter()));
	float distsq = glm::dot((m_destination - this->GetBoundCenter()), (m_destination - this->GetBoundCenter()));

	//rotang *= glm::sign(glm::cos(rotang));
	/*if (glm::abs(rotang) > 0.5f)
		this->RotateY(rotang);*/
	distsq = glm::dot((m_destination - this->GetBoundCenter()), (m_destination - this->GetBoundCenter()));

	std::vector<Unit*> units = m_scene->GetUnits();
	for (unsigned int i = 0; i < units.size(); i++)
	{
		if (this != units[i] && units[i]->GetTag() == 3)
		{
			float obstdistsq = glm::dot((units[i]->GetPosition() - m_position), (units[i]->GetPosition() - m_position));
			if (distsq > obstdistsq)
			{
				glm::vec3 obst = units[i]->GetBoundCenter() - this->GetBoundCenter();
				glm::vec3 obstext = units[i]->GetBoundExtents();
				glm::vec3 obsedge1 = glm::vec3(obst.x - obstext.x, 0.0f, obst.z - obstext.z);
				glm::vec3 obsedge2 = glm::vec3(obst.x + obstext.x, 0.0f, obst.z - obstext.z);
				glm::vec3 obsedge3 = glm::vec3(obst.x + obstext.x, 0.0f, obst.z + obstext.z);
				glm::vec3 obsedge4 = glm::vec3(obst.x - obstext.x, 0.0f, obst.z + obstext.z);
				glm::vec3 view = glm::vec3(m_orient[2]);
				float ang1 = glm::angle(view, glm::normalize(obsedge1 - this->GetBoundCenter()));
				float ang2 = glm::angle(view, glm::normalize(obsedge2 - this->GetBoundCenter()));
				float ang3 = glm::angle(view, glm::normalize(obsedge3 - this->GetBoundCenter()));
				float ang4 = glm::angle(view, glm::normalize(obsedge4 - this->GetBoundCenter()));
				float angmin = glm::min(glm::min(ang1, ang2), glm::min(ang3, ang4));
				float angmax = glm::max(glm::max(ang1, ang2), glm::max(ang3, ang4));

				//std::cout << ang1 << ", " << ang2 << ", " << ang3 << ", " << ang4 << std::endl;
				//float ang = glm::angle(view, glm::vec3());
				if (angmax > rotang && angmin < rotang)
				if (glm::abs(angmin) < glm::abs(angmax))
				{
					this->RotateY(angmax);
				}
				else
				{
					this->RotateY(angmin);
				}

				/*if (angmax > rotang && angmin < rotang)
				{
					if (angmax - rotang > rotang - angmin)
					{
						rotang = 90.0f;
					}
					else
					{
						rotang = -90.0f;
					}
					this->RotateY(rotang);
				}*/
				
				/*ang *= glm::sign(glm::cos(ang));
				if (glm::abs(ang) > 0.5f) this->RotateY(ang);*/
			}
		}
	}
	//rotang *= glm::sign(glm::cos(rotang));
	/*if (glm::abs(rotang) > 0.5f)
		this->RotateY(rotang);*/
	
	if (distsq > 2500.0f)
	{
		if (this->IsWalking() == false && m_state != ZOMBIE_DEATH) this->Walk();
	}
	else
	{
		if (this->IsAttacking() == false && m_state != ZOMBIE_DEATH) this->Attack();
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

void Zombie::Draw(unsigned int pass)
{
	m_model->SetTransform(glm::translate(glm::mat4(), m_position)  * m_orient);
	if (m_scene->IsFirstPass()) m_model->Animate(m_animation, m_state!=ZOMBIE_DEATH);
	m_model->Draw(pass);
}

void Zombie::Die()
{
	m_model->Transition(m_animation, ZOMBIE_DEATH, 0.08);
	m_state = ZOMBIE_DEATH;
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