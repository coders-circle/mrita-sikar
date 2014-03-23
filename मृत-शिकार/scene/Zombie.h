#pragma once
#include "LiveUnit.h"
#include "../audio/audio.h"

class Zombie : public LiveUnit
{
private:
	ModelAnimation m_animation;
	int m_state;
	irrklang::ISound *m_a_snoise, *m_a_madness, *m_a_pain;
	float m_walkspeed;
	float m_attackspeed;
	bool m_avoidingObstacle;
	bool m_attacked, m_reattack;

	std::unordered_set<Unit*> m_chaseUnits;
	LiveUnit * m_attackunit;

public:
	Zombie();
	void Reset();
	void Update(double timeDelta);
	void Draw();
	void InitAudio();
	void Die();
	bool TakeHit(int hitposition, glm::vec3 hitdirection, Unit * player);
	void Walk();
	void Idle();
	void Attack();
	void Flinch();
	bool IsAttacking();
	bool IsIdle();
	bool IsWalking();
	bool IsFlinching();
	bool IsDead();
	void SetSpeed(float walkspeed, float attackspeed = 1.0f) { m_walkspeed = walkspeed; m_attackspeed = attackspeed; }
	bool Attacked(Unit * &hitunit)
	{
		bool temp = m_attacked;
		m_attacked = false;
		hitunit = m_attackunit;
		return temp;
	}
};

