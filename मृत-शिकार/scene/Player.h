#pragma once
#include "LiveUnit.h"
#include "../audio/audio.h"

class Player : public LiveUnit
{
private:
	ModelAnimation m_animation;
	glm::vec3 m_offset;
	glm::mat4 m_offsetorient;
	//glm::mat3 m_orient_xonly;

	int m_state; bool m_inTransition;
	inline void ChangeState(int x);

	bool m_run, m_backrun;

	irrklang::ISoundSource* m_a_run;
	irrklang::ISound* m_a_running;
	irrklang::ISoundSource* m_a_endrun;
	irrklang::ISoundSource* m_a_shoot;
	irrklang::ISoundSource* m_a_shootdelayed;
	irrklang::vec3df m_a_pos;


public:
	Player();
	void Update(double deltaTime);
	void Draw();

	void Run(); void EndRun();
	void BackRun(); void EndBackRun();
	void StrafeLeft(); void EndStrafeLeft();
	void StrafeRight(); void EndStrafeRight();
	void Shoot();
	void InitAudio()
	{
		m_a_run = g_audioengine->addSoundSourceFromFile("sound/player/player_run.flac", irrklang::ESM_AUTO_DETECT, true);
		if (m_a_run)
		{
			m_a_run->setDefaultVolume(0.5f);
		}
		m_a_endrun = g_audioengine->addSoundSourceFromFile("sound/player/player_endrun.flac", irrklang::ESM_AUTO_DETECT, true);
		if (m_a_endrun)
		{
			m_a_endrun->setDefaultVolume(0.5f);
		}
		m_a_shootdelayed = g_audioengine->addSoundSourceFromFile("sound/weapon/pistol_shootdelayed.flac", irrklang::ESM_AUTO_DETECT, true);
		m_a_shoot = g_audioengine->addSoundSourceFromFile("sound/weapon/pistol_shoot.flac", irrklang::ESM_AUTO_DETECT, true);
	}

	void RotateX(float deltaX)
	{
		m_orient = glm::rotate(glm::mat4(), -deltaX, glm::vec3(0.0f, 1.0f, 0.0f)) * m_orient;
		//m_orient_xonly = static_cast<glm::mat3>(glm::rotate(glm::mat4(m_orient_xonly), -deltaX, glm::vec3(0.0f, 1.0f, 0.0f)));
	}
	bool IsRunning();
	/*void RotateY(float deltaY)
	{
		m_totalYRot += deltaY;
		if (m_totalYRot > 25.0f) m_totalYRot = 25.0f;
		else if (m_totalYRot < -40.0f) m_totalYRot = -40.0f;
		else m_orient = glm::rotate(glm::mat4(m_orient), deltaY, glm::vec3(1.0f, 0.0f, 0.0f));
	}*/
};

