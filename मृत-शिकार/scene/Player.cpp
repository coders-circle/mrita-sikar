#include "Player.h"
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

inline void Player::ChangeState(int x)
{
	m_state = x;
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

	if (x == PLAYER_RUNIDLE || x == PLAYER_IDLERUN || x == PLAYER_AIMIDLE || x == PLAYER_IDLEAIM)
		m_inTransition = true;
	else
		m_inTransition = false;
}

Player::Player() : m_state(PLAYER_IDLE), m_inTransition(false)
{
	m_orient = glm::rotate(glm::mat4(), 175.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	m_tag = 1;
}

void Player::Run()
{
	if (m_backrun) { m_run = true; m_backrun = false; }
	if (m_state == PLAYER_IDLE || m_state == PLAYER_AIM)
	{
		ChangeState(PLAYER_IDLERUN);
		m_run = true;
	}
}

void Player::EndRun()
{
	if (m_backrun) return;
	if (m_state == PLAYER_RUN || m_state == PLAYER_RUNSHOOTING || m_state == PLAYER_RUNAIMING)
	{
		ChangeState(PLAYER_RUNIDLE);
		m_run = false;
	}
}

void Player::BackRun()
{
	if (m_run) { m_backrun = true; m_run = false; }
	else if (m_state == PLAYER_IDLE || m_state == PLAYER_AIM)
	{
		ChangeState(PLAYER_IDLERUN);
		m_backrun = true;
	}
}

void Player::EndBackRun()
{
	if (m_run) return;
	if (m_state == PLAYER_RUN || m_state == PLAYER_RUNSHOOTING || m_state == PLAYER_RUNAIMING)
	{
		ChangeState(PLAYER_RUNIDLE);
		m_backrun = false;
	}
}

void Player::StrafeLeft()
{
	if (m_state == PLAYER_RUN || m_state == PLAYER_RUNSHOOTING || m_state == PLAYER_RUNAIMING)
		ChangeState(PLAYER_STRAFELEFT);
	else if (m_state == PLAYER_IDLE || m_state == PLAYER_AIM)
		ChangeState(PLAYER_IDLERUN);
}
void Player::EndStrafeLeft()
{
	if (m_state == PLAYER_STRAFELEFT) ChangeState(PLAYER_RUN);
	else if (m_state == PLAYER_SLEFTSHOOTING) ChangeState(PLAYER_RUNSHOOTING);
	else if (m_state == PLAYER_SLEFTAIMING) ChangeState(PLAYER_RUNAIMING);
}
void Player::StrafeRight()
{
	if (m_state == PLAYER_RUN || m_state == PLAYER_RUNSHOOTING || m_state == PLAYER_RUNAIMING)
		ChangeState(PLAYER_STRAFERIGHT);
	else if (m_state == PLAYER_IDLE || m_state == PLAYER_AIM)
		ChangeState(PLAYER_IDLERUN);
}
void Player::EndStrafeRight()
{
	if (m_state == PLAYER_STRAFERIGHT) ChangeState(PLAYER_RUN);
	else if (m_state == PLAYER_SRIGHTSHOOTING) ChangeState(PLAYER_RUNSHOOTING);
	else if (m_state == PLAYER_SRIGHTAIMING) ChangeState(PLAYER_RUNAIMING);
}

void Player::Shoot()
{
	switch (m_state)
	{
	case PLAYER_IDLE:
	case PLAYER_AIM:
		ChangeState(PLAYER_SHOOT);
		break;
		break;
	case PLAYER_RUN:
	case PLAYER_RUNAIMING:
		ChangeState(PLAYER_RUNSHOOTING);
		break;
	case PLAYER_STRAFELEFT:
	case PLAYER_SLEFTAIMING:
		ChangeState(PLAYER_SLEFTSHOOTING);
		break;
	case PLAYER_STRAFERIGHT:
	case PLAYER_SRIGHTAIMING:
		ChangeState(PLAYER_SRIGHTSHOOTING);
		break;
	}
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

	bool posChanged = false;
	glm::mat3 orient3x3(m_orient);
	switch (m_state)
	{
	case PLAYER_STRAFELEFT:
	case PLAYER_SLEFTAIMING:
	case PLAYER_SLEFTSHOOTING:
		m_position += orient3x3[0] * (float)deltaTime * 90.0f; posChanged = true;
		break;

	case PLAYER_STRAFERIGHT:
	case PLAYER_SRIGHTAIMING:
	case PLAYER_SRIGHTSHOOTING:
		m_position -= orient3x3[0] * (float)deltaTime * 90.0f; posChanged = true;
		break;
	}

	if (m_run)
	{
		m_position += orient3x3[2] * (float)deltaTime * 90.0f; posChanged = true;
	}
	else if (m_backrun)
	{
		m_position -= orient3x3[2] * (float)deltaTime * 90.0f; posChanged = true;
	}

	if (posChanged) UpdateBoundVolume();
	UnitCollections collisions;
	m_scene->GetPotentialCollisions(this, collisions);
	
	glm::vec3 out;
	for (unsigned int i = 0; i < collisions.size(); ++i)
	for (UnitIterator j = collisions[i]->begin(); j != collisions[i]->end(); ++j)
	{
		const Unit* other = *j;
		if (other->GetTag() == 2)
		if (m_scene->CheckPotentialCollision(this, other))
		if (GetBoundParent().IntersectBox(orient3x3, other->GetBoundParent(), glm::mat3(((LiveUnit*)other)->GetOrient()), &out))
		{
			m_position += out; UpdateBoundVolume();
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
