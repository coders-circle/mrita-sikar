#pragma once
#include "LiveUnit.h"
class People :	public LiveUnit
{
private:
	ModelAnimation m_animation;
	bool m_dead;
	float m_deadrotation;
	bool m_oncedead, m_once;
	glm::mat4 m_initialorient;
public:

	People();
	void Reset();
	void Update(double deltaTime);
	void Draw();

	void Die();
	bool IsDead() { return m_dead; }
};

