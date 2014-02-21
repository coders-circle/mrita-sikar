#pragma once

#include "Unit.h"


class WorldObject: public Unit
{
public:
	WorldObject()
	{
		m_tag = 3;	//obstacles
	}
	
};