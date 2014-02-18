#pragma once
#include "LiveUnit.h"
#include "../audio/audio.h"

class Zombie : public LiveUnit
{
private:
	ModelAnimation m_animation;
	int m_state;
	//irrklang::ISoundSource* m_a_noise;
	irrklang::ISound* m_a_snoise;
public:
	Zombie();
	void Update(double timeDelta);
	void Draw();
	void InitAudio();
	
	void Walk();
	void Idle();
	void Attack();
	void Flinch();
	bool IsAttacking();
	bool IsIdle();
	bool IsWalking();

};

