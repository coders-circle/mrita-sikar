#pragma once
#include "LiveUnit.h"
#include "Camera.h"

class TPCamera : public Camera
{
private:
	LiveUnit * m_target;
	float m_distance;
public:
	void UpdateView(double deltaTime);
	void SetParameters(LiveUnit * target, float distance)
	{
		m_target = target; m_distance = distance;
	}
};

