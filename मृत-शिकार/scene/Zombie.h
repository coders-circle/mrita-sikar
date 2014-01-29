#pragma once
#include "LiveUnit.h"

class Zombie : public LiveUnit
{
private:
	ModelAnimation m_animation;
	int m_state;
public:
	Zombie();
	void Update(double timeDelta);
	void Draw();
};

