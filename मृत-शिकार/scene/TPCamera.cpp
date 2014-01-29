#include "TPCamera.h"

void TPCamera::UpdateView(double deltaTime)
{
	glm::vec3 tpos = m_target->GetPosition();
	tpos = glm::vec3(tpos.x, tpos.y+50.0f, tpos.z);
	m_view = glm::lookAt(
		tpos - m_target->GetOrient()[2] * m_distance, //glm::vec3(0.0f, 0.0f, 5.0f), 
		tpos,						//glm::vec3(0.0f, 0.0f, 0.0f), 
		m_target->GetOrient()[1]	//glm::vec3(0.0f, 1.0f, 0.0f)
		);
}