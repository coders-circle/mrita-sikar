#pragma once
#include "../graphics/graphics.h"

class Camera
{
protected:
	glm::mat4 m_view, m_projection;
	float nh, nw, fh, fw, nearD, farD;
	glm::vec3 m_points[8];
	Box m_boundBox;

	glm::mat4 LookAt(glm::vec3 eye, glm::vec3 at, glm::vec3 up)
	{
		// compute the Z axis of camera
		// this axis points in the opposite direction from 
		// the looking direction
		glm::vec3  Z = glm::normalize(eye - at);

		// X axis of camera with given "up" vector and Z axis
		glm::vec3 X = glm::normalize(up * Z);

		// the real "up" vector is the cross product of Z and X
		glm::vec3 Y = glm::cross(Z, X);

		// compute the centers of the near and far planes
		glm::vec3 nc = eye - Z * nearD;
		glm::vec3 fc = eye - Z * farD;

		// compute the 4 corners of the frustum on the near plane
		m_points[0] = nc + Y * nh - X * nw;
		m_points[1] = nc + Y * nh + X * nw;
		m_points[2] = nc - Y * nh - X * nw;
		m_points[3] = nc - Y * nh + X * nw;

		// compute the 4 corners of the frustum on the far plane
		m_points[4] = fc + Y * fh - X * fw;
		m_points[5] = fc + Y * fh + X * fw;
		m_points[6] = fc - Y * fh - X * fw;
		m_points[7] = fc - Y * fh + X * fw;

		glm::vec3 min=m_points[0], max=m_points[0];
		for (int i = 1; i < 8; ++i)
		for (int j = 0; j < 3; ++j)
		{
			if (m_points[i][j] < min[j]) min[j] = m_points[i][j];
			if (m_points[i][j] > max[j]) max[j] = m_points[i][j];
		}
		m_boundBox.SetCenter((max + min) / 2.0f);
		m_boundBox.SetExtents((max - min) / 2.0f);

		return glm::lookAt(eye, at, up);
	}
public:
	virtual void UpdateView(double deltaTime)
	{
		m_view = LookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	virtual void UpdateProjection(float width, float height)
	{
		nearD = 0.1f; farD = 5000.0f;
		m_projection = glm::perspective(45.0f, width / height, nearD, farD);

		float tang = glm::tan(glm::radians(45.0f * 0.5f));
		nh = nearD * tang;
		nw = nh * width/height;
		fh = farD  * tang;
		fw = fh * width / height;
	}

	const glm::mat4& GetView()			{ return m_view; }
	const glm::mat4& GetProjection()	{ return m_projection; }

	const Box &GetBoundBox() const
	{
		return m_boundBox;
	}
};