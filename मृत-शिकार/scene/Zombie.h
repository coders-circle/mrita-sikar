#pragma once
#include "LiveUnit.h"
#include "../audio/audio.h"

class Zombie : public LiveUnit
{
private:
	ModelAnimation m_animation;
	int m_state;
	irrklang::ISound *m_a_snoise, *m_a_madness, *m_a_pain;
	glm::vec3 m_destination;
	float m_walkspeed;
	float m_attackspeed;
	bool m_avoidingObstacle;
	int GetRand(int range){ return rand() % range; }
public:
	Zombie();
	void Update(double timeDelta);
	void Draw();
	void InitAudio();
	void SetDestination(glm::vec3);
	void Die();
	void TakeHit(int hitposition, glm::vec3 hitdirection);
	void Walk();
	void Idle();
	void Attack();
	void Flinch();
	bool IsAttacking();
	bool IsIdle();
	bool IsWalking();
	void SetSpeed(float walkspeed, float attackspeed = 1.0f) { m_walkspeed = walkspeed; m_attackspeed = attackspeed; }
};

