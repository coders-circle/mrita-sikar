#pragma once
#include "LiveUnit.h"
#include "../audio/audio.h"

class Player : public LiveUnit
{
private:
	ModelAnimation m_animation;
	glm::vec3 m_offset;
	glm::mat4 m_offsetorient;
	int m_state; bool m_inTransition;
	inline void ChangeState(int x);
	bool m_run, m_backrun;

	int m_totalAmmo;
	int m_ammoLeft;
	int m_ammoCapacity;

	irrklang::ISoundSource* m_a_run;
	irrklang::ISound* m_a_running;
	irrklang::ISoundSource* m_a_endrun;
	irrklang::ISoundSource* m_a_shoot;
	irrklang::ISoundSource* m_a_shootdelayed;
	irrklang::ISoundSource* m_a_hit1, *m_a_hit2, *m_a_hit3;
	irrklang::ISoundSource* m_a_breath;
	irrklang::ISoundSource* m_a_pain;
	irrklang::ISound* m_a_breathing;



	irrklang::vec3df m_a_pos;

	//SoundSource m_sound;


public:
	Player();
	void Update(double deltaTime);
	void Draw();

	void Run(); void EndRun();
	void BackRun(); void EndBackRun();
	void StrafeLeft(); void EndStrafeLeft();
	void StrafeRight(); void EndStrafeRight();
	void Shoot();
	void Reload();
	void InitAudio();
	void RotateX(float deltaX)
	{
		m_orient = glm::rotate(glm::mat4(), -deltaX, glm::vec3(0.0f, 1.0f, 0.0f)) * m_orient;
	}
	bool IsRunning();
	void TakeHit();
};

