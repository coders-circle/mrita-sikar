#include "Player.h"
#include "Zombie.h"
#include "Scene.h"


enum PlayerStates { 
	PLAYER_IDLE = 0, PLAYER_RUN, PLAYER_SHOOT, 
	PLAYER_AIM, 
	PLAYER_STRAFELEFT, PLAYER_STRAFERIGHT,
	PLAYER_AIMIDLE, PLAYER_IDLEAIM, 
	PLAYER_GUNRELOAD, 
	PLAYER_IDLERUN, PLAYER_RUNIDLE, 
	PLAYER_RUNSHOOTING, PLAYER_RUNAIMING,
	PLAYER_SLEFTSHOOTING, PLAYER_SRIGHTSHOOTING, 
	PLAYER_SLEFTAIMING, PLAYER_SRIGHTAIMING,
};

enum PlayerSounds {
	SND_PLAYER_RUN = 0,
	SND_PLAYER_ENDRUN,
	SND_PLAYER_SHOOT,
	SND_PLAYER_HIT1, SND_PLAYER_HIT2, SND_PLAYER_HIT3,
	SND_PLAYER_DIE1, SND_PLAYER_DIE2, SND_PLAYER_DIE3
};

inline void Player::ChangeState(int x)
{
	switch (x)
	{
	case PLAYER_RUNSHOOTING:
	case PLAYER_RUNAIMING:
	case PLAYER_SLEFTSHOOTING:
	case PLAYER_SRIGHTSHOOTING:
	case PLAYER_SLEFTAIMING:
	case PLAYER_SRIGHTAIMING:
		m_offsetorient = glm::rotate(glm::mat4(), -1.5f, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(glm::mat4(), 18.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		m_offset = glm::vec3(0.0f, 0.0f, -25.0f);
		break;
	default:
		m_offsetorient = glm::mat4();	m_offset = glm::vec3();
	}

	if (x == PLAYER_STRAFELEFT || x == PLAYER_STRAFERIGHT)
		m_model->Transition(m_animation, x, 0.18);
	else
		m_model->Transition(m_animation, x, 0.0);


	m_state = x;

	if (x == PLAYER_RUNIDLE || x == PLAYER_IDLERUN || x == PLAYER_AIMIDLE || x == PLAYER_IDLEAIM)
		m_inTransition = true;
	else
		m_inTransition = false;
}

Player::Player() : m_state(PLAYER_IDLE), m_inTransition(false), m_camera(NULL)
{
	m_orient = glm::rotate(glm::mat4(), 175.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	m_tag = 1;
}

void Player::InitAudio()
{

	/*m_sound.Initialize(g_audioengine, m_position);
	m_sound.AddSource("sound/player/player_run.flac");
	m_sound.AddSource("sound/player/player_endrun.flac");
	m_sound.AddSource("sound/player/player_hit1.mp3");*/


	m_a_run = g_audioengine->addSoundSourceFromFile("sound/player/player_run.flac", irrklang::ESM_AUTO_DETECT, true);
	if (m_a_run) m_a_run->setDefaultVolume(0.2f);
	m_a_endrun = g_audioengine->addSoundSourceFromFile("sound/player/player_endrun.flac", irrklang::ESM_AUTO_DETECT, true);
	if (m_a_endrun) m_a_endrun->setDefaultVolume(0.2f);
	m_a_shootdelayed = g_audioengine->addSoundSourceFromFile("sound/weapon/pistol_shootdelayed.flac", irrklang::ESM_AUTO_DETECT, true);
	m_a_shoot = g_audioengine->addSoundSourceFromFile("sound/weapon/pistol_shoot.flac", irrklang::ESM_AUTO_DETECT, true);

	//m_a_hit1 = g_audioengine->addSoundSourceFromFile("sound/player/player_hit1.mp3", irrklang::ESM_AUTO_DETECT, true);
	m_a_hit2 = g_audioengine->addSoundSourceFromFile("sound/player/player_hit2.mp3", irrklang::ESM_AUTO_DETECT, true);
	m_a_hit3 = g_audioengine->addSoundSourceFromFile("sound/player/player_hit3.mp3", irrklang::ESM_AUTO_DETECT, true);

	m_a_breath = g_audioengine->addSoundSourceFromFile("sound/player/player_breath.mp3", irrklang::ESM_AUTO_DETECT, true);
	if (m_a_breath) m_a_breath->setDefaultVolume(0.1f);
	if (m_a_shoot) m_a_shoot->setDefaultVolume(0.2f);
	if (m_a_shootdelayed) m_a_shootdelayed->setDefaultVolume(0.2f);
	m_a_breathing = g_audioengine->play2D(m_a_breath, true, false, true);
}

bool Player::IsRunning()
{
	return m_state == PLAYER_RUN;
}

void Player::Run()
{
	if (m_state != PLAYER_IDLEAIM && m_state != PLAYER_SHOOT)
		m_run = true;
	if (m_backrun) { m_backrun = false; }
	if (m_state == PLAYER_IDLE || m_state == PLAYER_AIM)
	{
		ChangeState(PLAYER_IDLERUN);
		if (!g_audioengine->isCurrentlyPlaying(m_a_run))
		{
			m_a_running = g_audioengine->play2D(m_a_run, true, false, true);
			//m_a_running = g_audioengine->play3D(m_a_run, irrklang::vec3df(m_position.x, m_position.y, m_position.z), true, false, true);
		}
	}
}

void Player::EndRun()
{
	if (m_backrun) return;
	m_run = false;
	if (m_state == PLAYER_RUN || m_state == PLAYER_RUNSHOOTING || m_state == PLAYER_RUNAIMING)
	{
		ChangeState(PLAYER_RUNIDLE);
		if (m_a_running)
		{
			m_a_running->stop();
			if (!g_audioengine->isCurrentlyPlaying(m_a_endrun))
				g_audioengine->play2D(m_a_endrun);
		}
	}
}

void Player::BackRun()
{
	if (m_state != PLAYER_IDLEAIM && m_state != PLAYER_SHOOT)
		m_backrun = true;
	if (m_run) {  m_run = false; }
	if (m_state == PLAYER_IDLE || m_state == PLAYER_AIM)
	{
		ChangeState(PLAYER_IDLERUN);
		if (!g_audioengine->isCurrentlyPlaying(m_a_run))
		{
			m_a_running = g_audioengine->play2D(m_a_run, true, false, true);
		}
	}
}

void Player::EndBackRun()
{
	if (m_run) return;
	m_backrun = false;
	if (m_state == PLAYER_RUN || m_state == PLAYER_RUNSHOOTING || m_state == PLAYER_RUNAIMING)
	{
		ChangeState(PLAYER_RUNIDLE);
		if (m_a_running)
		{
			m_a_running->stop();
			if (!g_audioengine->isCurrentlyPlaying(m_a_endrun))
				g_audioengine->play2D(m_a_endrun);
		}
		
	}
}

void Player::StrafeLeft()
{
	if (m_state == PLAYER_RUN || m_state == PLAYER_RUNSHOOTING || m_state == PLAYER_RUNAIMING)
		ChangeState(PLAYER_STRAFELEFT);
	else if (m_state == PLAYER_IDLE || m_state == PLAYER_AIM)
		ChangeState(PLAYER_IDLERUN);
	if (!g_audioengine->isCurrentlyPlaying(m_a_run))
	{
		m_a_running = g_audioengine->play2D(m_a_run, true, false, true);
	}
}
void Player::EndStrafeLeft()
{
	if (m_state == PLAYER_STRAFELEFT) ChangeState(PLAYER_RUN);
	else if (m_state == PLAYER_SLEFTSHOOTING) ChangeState(PLAYER_RUNSHOOTING);
	else if (m_state == PLAYER_SLEFTAIMING) ChangeState(PLAYER_RUNAIMING);
	if (m_state == PLAYER_IDLE || m_state == PLAYER_IDLEAIM || m_state == PLAYER_SHOOT || m_state == PLAYER_AIMIDLE)
	{
		if (m_a_running)
		{
			m_a_running->stop();
			m_a_running->drop();
			m_a_running = 0;
			if (!g_audioengine->isCurrentlyPlaying(m_a_endrun))
				g_audioengine->play2D(m_a_endrun);
		}
	}
}
void Player::StrafeRight()
{
	if (m_state == PLAYER_RUN || m_state == PLAYER_RUNSHOOTING || m_state == PLAYER_RUNAIMING)
		ChangeState(PLAYER_STRAFERIGHT);
	else if (m_state == PLAYER_IDLE || m_state == PLAYER_AIM)
		ChangeState(PLAYER_IDLERUN);
	if (!g_audioengine->isCurrentlyPlaying(m_a_run))
	{
		m_a_running = g_audioengine->play2D(m_a_run, true, false, true);
	}
}
void Player::EndStrafeRight()
{
	if (m_state == PLAYER_STRAFERIGHT) ChangeState(PLAYER_RUN);
	else if (m_state == PLAYER_SRIGHTSHOOTING) ChangeState(PLAYER_RUNSHOOTING);
	else if (m_state == PLAYER_SRIGHTAIMING) ChangeState(PLAYER_RUNAIMING);
	if (m_state == PLAYER_IDLE || m_state == PLAYER_IDLEAIM || m_state == PLAYER_SHOOT || m_state == PLAYER_AIMIDLE)
	{
		if (m_a_running)
		{
			m_a_running->stop();
			m_a_running->drop();
			m_a_running = 0;
			if (!g_audioengine->isCurrentlyPlaying(m_a_endrun))
				g_audioengine->play2D(m_a_endrun);
		}
	}
}

bool Player::Shoot()
{
	switch (m_state)
	{
	case PLAYER_IDLE:
		g_audioengine->play2D(m_a_shootdelayed);
		ChangeState(PLAYER_SHOOT);
		break;
	case PLAYER_AIM:
		ChangeState(PLAYER_SHOOT);
		g_audioengine->play2D(m_a_shoot);
		break;
	case PLAYER_RUN:
		ChangeState(PLAYER_RUNSHOOTING);
		g_audioengine->play2D(m_a_shootdelayed);
		break;
	case PLAYER_RUNAIMING:
		ChangeState(PLAYER_RUNSHOOTING);
		g_audioengine->play2D(m_a_shoot);
		break;
	case PLAYER_STRAFELEFT:
		ChangeState(PLAYER_SLEFTSHOOTING);
		g_audioengine->play2D(m_a_shootdelayed);
		break;
	case PLAYER_SLEFTAIMING:
		ChangeState(PLAYER_SLEFTSHOOTING);
		g_audioengine->play2D(m_a_shoot);
		break;
	case PLAYER_STRAFERIGHT:
		ChangeState(PLAYER_SRIGHTSHOOTING);
		g_audioengine->play2D(m_a_shootdelayed);
		break;
	case PLAYER_SRIGHTAIMING:
		ChangeState(PLAYER_SRIGHTSHOOTING);
		g_audioengine->play2D(m_a_shoot);
		break;
	default:
		return false;
	}
	return true;
}

void Player::Update(double deltaTime)
{
	bool end = false;
	if (m_inTransition)
		m_model->Advance(m_animation, deltaTime * 3, &end);		
	else
		m_model->Advance(m_animation, deltaTime, &end);

	if (end)
	{	
		switch (m_state)
		{
		case PLAYER_RUNIDLE:
		case PLAYER_AIMIDLE:
			ChangeState(PLAYER_IDLE);			break; 
		case PLAYER_IDLERUN:
		case PLAYER_RUNAIMING:
			ChangeState(PLAYER_RUN);			break;
		case PLAYER_AIM:
			ChangeState(PLAYER_AIMIDLE);		break;
		case PLAYER_IDLEAIM:
		case PLAYER_SHOOT:
			ChangeState(PLAYER_AIM);			break;
		
		case PLAYER_RUNSHOOTING:
			ChangeState(PLAYER_RUNAIMING);		break;
		case PLAYER_SLEFTSHOOTING:
			ChangeState(PLAYER_SLEFTAIMING);	break;
		case PLAYER_SRIGHTSHOOTING:
			ChangeState(PLAYER_SRIGHTAIMING);	break;

		case PLAYER_SLEFTAIMING:
			ChangeState(PLAYER_STRAFELEFT);		break;
		case PLAYER_SRIGHTAIMING:
			ChangeState(PLAYER_STRAFERIGHT);	break;
		}
	}

	const float deltaPos = 90.0f;

	bool posChanged = false;
	glm::mat3 orient3x3(m_orient);
	float posboost = 1.5f;
	switch (m_state)
	{
	case PLAYER_STRAFELEFT:
	case PLAYER_SLEFTAIMING:
	case PLAYER_SLEFTSHOOTING:
		m_position += orient3x3[0] * (float)deltaTime * deltaPos *posboost; posChanged = true;
		break;

	case PLAYER_STRAFERIGHT:
	case PLAYER_SRIGHTAIMING:
	case PLAYER_SRIGHTSHOOTING:
		m_position -= orient3x3[0] * (float)deltaTime * deltaPos*posboost; posChanged = true;
		break;
	}

	if (m_run)
	{
		m_position += orient3x3[2] * (float)deltaTime * deltaPos*posboost; posChanged = true;
	}
	else if (m_backrun)
	{
		m_position -= orient3x3[2] * (float)deltaTime * deltaPos*posboost; posChanged = true;
	}

	if (posChanged)
	{
		UpdateBoundVolume();
		if (m_a_breathing)
		{
			m_a_breathing->setPosition(irrklang::vec3df(m_position.x, m_position.y, m_position.z));
		}
	}
	UnitCollections collisions;
	m_scene->GetPotentialCollisions(this, collisions);
	
	for (unsigned int i = 0; i < collisions.size(); ++i)
	{
		Unit* other = collisions[i];
		if (other != this)
		{
			if (other->IsLiveUnit())
			{
				Collide((LiveUnit*)other);
			if (other->GetTag() == 2)
				if (((Zombie*)other)->IsAttacking())
					Collide(other->GetBoundChild(3), glm::mat3(other->GetOrient()));
			}
			else
				Collide(other);
		}
	}
	
}

static glm::mat4 g_globaltransform = glm::scale(glm::mat4(), glm::vec3(1/4.0f));
void Player::Draw()
{
	m_model->SetTransform(glm::translate(glm::mat4(), m_position)  * m_orient * g_globaltransform
		* glm::translate(glm::mat4(), m_offset) * m_offsetorient);
	m_model->Animate(m_animation);
	m_model->Draw();	
}

void Player::TakeHit()
{
	m_health -= 0;
	if (m_camera) m_camera->Shake();
	switch (GetRand(3))
	{
	case 0: g_audioengine->play2D(m_a_hit1); break;
	case 1: g_audioengine->play2D(m_a_hit2); break;
	default: g_audioengine->play2D(m_a_hit3);
	}
}
