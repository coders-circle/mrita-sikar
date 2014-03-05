#pragma once
#include "LiveUnit.h"
#include "../audio/audio.h"

class Zombie : public LiveUnit
{
private:
	ModelAnimation m_animation;
	int m_state;
	irrklang::ISound* m_a_snoise;
	glm::vec3 m_destination;
public:
	Zombie();
	void Update(double timeDelta);
	void Draw();
	void InitAudio();

	void SetDestination(glm::vec3);

	void Die();

	void Walk();
	void Idle();
	void Attack();
	void Flinch();
	bool IsAttacking();
	bool IsIdle();
	bool IsWalking();


};

