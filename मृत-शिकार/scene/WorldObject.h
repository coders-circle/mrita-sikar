#pragma once

#include "Unit.h"


class WorldObject: public Unit
{
private:
	glm::mat4 m_defaultrotation;
public:
	WorldObject()
	{
		m_tag = 3;	//obstacles
	}
	void SetDefaultRotation(glm::mat4 defaultrotation)
	{
		m_defaultrotation = defaultrotation;
	}
	void AddToScene(GameScene* scene)
	{
		scene->AddUnit(this);
	}
};