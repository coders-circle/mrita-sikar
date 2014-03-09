/*
COLLISION.CPP
-----------------------------
*/

#include "Collision.h"

bool Box::IntersectBox(const Box &box2, glm::vec3 * out) const
{
	float d[3]; int i = 0;
	d[0] = (m_extents[0] + box2.m_extents[0]) - fabs(m_center.x - box2.m_center.x);
	if (d[0] <= 0) return false;
	d[1] = (m_extents[1] + box2.m_extents[1]) - fabs(m_center.y - box2.m_center.y);
	if (d[1] <= 0) return false;
	d[2] = (m_extents[2] + box2.m_extents[2]) - fabs(m_center.z - box2.m_center.z);
	if (d[2] <= 0) return false;

	if (out)
	{
		if (d[1]<d[0]) i = 1;
		if (d[2]<d[i]) i = 2;
		*out = glm::vec3(0.0f);
		(*out)[i] = -d[i] * SIGN(box2.m_center[i] - m_center[i]);
	}
	return true;
}

bool Ray::IntersectBox(const Box &box, float &tmin) const
{
	tmin = 0.0f; float tmax = FLT_MAX;
	for (int i = 0; i < 3; i++) {
		float amin = box.GetCenter()[i] - box.GetExtents()[i], amax = box.GetCenter()[i] + box.GetExtents()[i];
		if (fabs(m_direction[i]) < FLT_EPSILON) {
			// Ray is parallel to slab. No hit if origin not within slab
			if (m_origin[i] < amin || m_origin[i] > amax) return false;
		}
		else {
			// Compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / m_direction[i];
			float t1 = (amin - m_origin[i]) * ood;
			float t2 = (amax - m_origin[i]) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) Swap(t1, t2);
			// Compute the intersection of slab intersection intervals
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;		// in book it is given (t2 > tmax) but seems to be wrong according to text

			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return false;
		}
	}
	/*
	Vector m = p - s.c;
	float b = Dot(m, d);
	float c = Dot(m, m) - s.r * s.r;
	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
	if (c > 0.0f && b > 0.0f) return 0;
	float discr = b*b - c;
	// A negative discriminant corresponds to ray missing sphere
	if (discr < 0.0f) return 0;
	// Ray now found to intersect sphere, compute smallest t value of intersection
	t = -b - Sqrt(discr);
	// If t is negative, ray started inside sphere so clamp t to zero
	if (t < 0.0f) t = 0.0f;
	q = p + t * d;
	return 1;
	*/


	return true;
}
bool Ray::IntersectRect(const Rect &rect) const
{
	float tmin = 0.0f, tmax = FLT_MAX;
	for (int i = 0; i < 2; i++) {
		//float amin = (i == 0) ? rect.x : rect.y,	amax = (i == 0) ? rect.x + rect.width : rect.y + rect.height;
		float amin = *(&(rect.x) + i), amax = *(&(rect.x) + i) + *(&(rect.width) + i);

		if (fabs(m_direction[i]) < FLT_EPSILON) {
			if (m_origin[i] < amin || m_origin[i] > amax) return false;
		}
		else {
			float ood = 1.0f / m_direction[i];
			float t1 = (amin - m_origin[i]) * ood;
			float t2 = (amax - m_origin[i]) * ood;
			if (t1 > t2) Swap(t1, t2);
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
			if (tmin > tmax) return false;
		}
	}
	return true;
}


bool Box::IntersectBox(const glm::mat3 &orient1, const Box &box2, const glm::mat3 &orient2, glm::vec3 * out) const
{
	float ra, rb;   // projected radii
	glm::mat3 R, AbsR;

	for (int i = 0; i<3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = glm::dot(orient1[i], orient2[j]);
			AbsR[i][j] = fabs(R[i][j]) + FLT_EPSILON;
		}
	}

	glm::vec3 t = box2.m_center - m_center;
	// change translation to box1's coordinate frame
	t = glm::vec3(glm::dot(t, orient1[0]), glm::dot(t, orient1[1]), glm::dot(t, orient1[2]));

	float dis, dmin; glm::vec3 dir = orient1[0];
	// Test axes: A0, A1, A2
	for (int i = 0; i<3; i++)
	{
		ra = m_extents[i];
		rb = box2.m_extents[0] * AbsR[i][0] + box2.m_extents[1] * AbsR[i][1] + box2.m_extents[2] * AbsR[i][2];
		dis = fabs(t[i]) - (ra + rb);
		if (dis > 0) return false;

		if (out)
		{
			if (i == 0) dmin = -dis;
			else
			{
				if (-dis < dmin)
				{
					dmin = -dis;
					dir = orient1[i];
				}
			}
		}
	}

	// Test axes: B0, B1, B2
	for (int i = 0; i<3; i++)
	{
		ra = m_extents[0] * AbsR[0][i] + m_extents[1] * AbsR[1][i] + m_extents[2] * AbsR[2][i];
		rb = box2.m_extents[i];
		dis = fabs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) - (ra + rb);
		if (dis > 0) return false;

		if (out)
		if (-dis < dmin)
		{
			dmin = -dis;
			dir = orient2[i];
		}
	}

	float l; glm::vec3 tdir;
	// 9 perpendicular axes: glm::cross product of A0, A1, A2 with B0, B1, B2
	ra = m_extents[1] * AbsR[2][0] + m_extents[2] * AbsR[1][0];
	rb = box2.m_extents[1] * AbsR[0][2] + box2.m_extents[2] * AbsR[0][1];
	dis = fabs(t[2] * R[1][0] - t[1] * R[2][0]) - (ra + rb);
	if (dis > 0) return false;
	if (out)
	{
		tdir = glm::cross(orient1[0], orient2[0]);
		l = tdir.x*tdir.x + tdir.y*tdir.y + tdir.z*tdir.z;
		if (l>0)
		{
			dis /= sqrtf(l);
			if (-dis < dmin)
			{
				dmin = -dis;
				dir = tdir;
			}
		}
	}

	ra = m_extents[1] * AbsR[2][1] + m_extents[2] * AbsR[1][1];
	rb = box2.m_extents[0] * AbsR[0][2] + box2.m_extents[2] * AbsR[0][0];
	dis = fabs(t[2] * R[1][1] - t[1] * R[2][1]) - (ra + rb);
	if (dis > 0) return false;
	if (out)
	{
		tdir = glm::cross(orient1[0], orient2[1]);
		l = tdir.x*tdir.x + tdir.y*tdir.y + tdir.z*tdir.z;
		if (l>0)
		{
			dis /= sqrtf(l);
			if (-dis < dmin)
			{
				dmin = -dis;
				dir = tdir;
			}
		}
	}

	ra = m_extents[1] * AbsR[2][2] + m_extents[2] * AbsR[1][2];
	rb = box2.m_extents[0] * AbsR[0][1] + box2.m_extents[1] * AbsR[0][0];
	dis = fabs(t[2] * R[1][2] - t[1] * R[2][2]) - (ra + rb);
	if (dis > 0) return false;
	if (out)
	{
		tdir = glm::cross(orient1[0], orient2[2]);
		l = tdir.x*tdir.x + tdir.y*tdir.y + tdir.z*tdir.z;
		if (l>0)
		{
			dis /= sqrtf(l);
			if (-dis < dmin)
			{
				dmin = -dis;
				dir = tdir;
			}
		}
	}

	ra = m_extents[0] * AbsR[2][0] + m_extents[2] * AbsR[0][0];
	rb = box2.m_extents[1] * AbsR[1][2] + box2.m_extents[2] * AbsR[1][1];
	dis = fabs(t[0] * R[2][0] - t[2] * R[0][0]) - (ra + rb);
	if (dis > 0) return false;
	if (out)
	{
		tdir = glm::cross(orient1[1], orient2[0]);
		l = tdir.x*tdir.x + tdir.y*tdir.y + tdir.z*tdir.z;
		if (l>0)
		{
			dis /= sqrtf(l);
			if (-dis < dmin)
			{
				dmin = -dis;
				dir = tdir;
			}
		}
	}

	ra = m_extents[0] * AbsR[2][1] + m_extents[2] * AbsR[0][1];
	rb = box2.m_extents[0] * AbsR[1][2] + box2.m_extents[2] * AbsR[1][0];
	dis = fabs(t[0] * R[2][1] - t[2] * R[0][1]) - (ra + rb);
	if (dis > 0) return false;
	if (out)
	{
		tdir = glm::cross(orient1[1], orient2[1]);
		l = tdir.x*tdir.x + tdir.y*tdir.y + tdir.z*tdir.z;
		if (l>0)
		{
			dis /= sqrtf(l);
			if (-dis < dmin)
			{
				dmin = -dis;
				dir = tdir;
			}
		}
	}

	ra = m_extents[0] * AbsR[2][2] + m_extents[2] * AbsR[0][2];
	rb = box2.m_extents[0] * AbsR[1][1] + box2.m_extents[1] * AbsR[1][0];
	dis = fabs(t[0] * R[2][2] - t[2] * R[0][2]) - (ra + rb);
	if (dis > 0) return false;
	if (out)
	{
		tdir = glm::cross(orient1[1], orient2[2]);
		l = tdir.x*tdir.x + tdir.y*tdir.y + tdir.z*tdir.z;
		if (l>0)
		{
			dis /= sqrtf(l);
			if (-dis < dmin)
			{
				dmin = -dis;
				dir = tdir;
			}
		}
	}

	ra = m_extents[0] * AbsR[1][0] + m_extents[1] * AbsR[0][0];
	rb = box2.m_extents[1] * AbsR[2][2] + box2.m_extents[2] * AbsR[2][1];
	dis = fabs(t[1] * R[0][0] - t[0] * R[1][0]) - (ra + rb);
	if (dis > 0) return false;
	if (out)
	{
		tdir = glm::cross(orient1[2], orient2[0]);
		l = tdir.x*tdir.x + tdir.y*tdir.y + tdir.z*tdir.z;
		if (l>0)
		{
			dis /= sqrtf(l);
			if (-dis < dmin)
			{
				dmin = -dis;
				dir = tdir;
			}
		}
	}

	ra = m_extents[0] * AbsR[1][1] + m_extents[1] * AbsR[0][1];
	rb = box2.m_extents[0] * AbsR[2][2] + box2.m_extents[2] * AbsR[2][0];
	dis = fabs(t[1] * R[0][1] - t[0] * R[1][1]) - (ra + rb);
	if (dis > 0) return false;
	if (out)
	{
		tdir = glm::cross(orient1[2], orient2[1]);
		l = tdir.x*tdir.x + tdir.y*tdir.y + tdir.z*tdir.z;
		if (l>0)
		{
			dis /= sqrtf(l);
			if (-dis < dmin)
			{
				dmin = -dis;
				dir = tdir;
			}
		}
	}

	ra = m_extents[0] * AbsR[1][2] + m_extents[1] * AbsR[0][2];
	rb = box2.m_extents[0] * AbsR[2][1] + box2.m_extents[1] * AbsR[2][0];
	dis = fabs(t[1] * R[0][2] - t[0] * R[1][2]) - (ra + rb);
	if (dis > 0) return false;
	if (out)
	{
		tdir = glm::cross(orient1[2], orient2[2]);
		l = tdir.x*tdir.x + tdir.y*tdir.y + tdir.z*tdir.z;
		if (l>0)
		{
			dis /= sqrtf(l);
			if (-dis < dmin)
			{
				dmin = -dis;
				dir = tdir;
			}
		}

		if (glm::dot(m_center - box2.m_center, dir)<0) dir = -dir;
		*out = glm::normalize(dir) * (dmin + FLT_EPSILON);
	}

	return true;
}