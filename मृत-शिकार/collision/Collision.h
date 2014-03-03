#pragma once
#include "../graphics/stdinc.h"

#define SIGN(x) (((x)<0?-1.0f:1.0f))

template <typename T>
inline void Swap(T &x, T &y) { T temp; temp = x; x = y; y = temp; };

/*
A 2-d rectangle
*/
struct Rect
{
	float x, y, width, height;
	Rect(float x_, float y_, float w_, float h_) :x(x_), y(y_), width(w_), height(h_) {}
	Rect(){}
};

/*
A bounding box 
- Center is the position of the box
- Extents is the half-length of sides in each direction from the center
*/
class Box
{
private:
	glm::vec3 m_center, m_extents;
public:
	Box(const glm::vec3 &center = glm::vec3(0.0f), const glm::vec3 &extents = glm::vec3(0.0f)) : m_center(center), m_extents(extents) {}

	const glm::vec3& GetCenter() const
	{ return m_center; }
	const glm::vec3& GetExtents() const
	{ return m_extents; }

	void SetCenter(const glm::vec3 &center){ m_center = center; }
	void SetExtents(const glm::vec3 &extents){ m_extents = extents; }

	/*Check if box intersects another box*/
	bool IntersectBox(const Box &box2, glm::vec3 * out = NULL) const;
	/*Check if box intersects another box considering both boxes are rotated
	Use glm::mat3() for orient parameter if one of the boxes is not rotated*/
	bool IntersectBox(const glm::mat3 &orient1, const Box &box2, const glm::mat3 &orient2, glm::vec3 * out = NULL) const;

	/*Get a 2d-rectangle (xz-values only) from the box*/
	Rect GetRect() const
	{
		return Rect(m_center.x-m_extents.x, m_center.z-m_extents.z, m_extents.x*2.0f, m_extents.z*2.0f);
	}
};

/*
A Ray
- Origin is the origin point of the ray
- Direction is where it's pointing to
*/
class Ray
{
private:
	glm::vec3 m_origin, m_direction;
public:
	Ray(const glm::vec3 &origin = glm::vec3(0.0f), const glm::vec3 &direction = glm::vec3(0.0f)) : m_origin(origin), m_direction(direction) {}
	
	const glm::vec3& GetOrigin() const
	{ return m_origin; }
	const glm::vec3& GetDirection() const
	{ return m_direction; }

	void SetOrigin(const glm::vec3& origin) { m_origin = origin; }
	void SetDirection(const glm::vec3& direction) { m_direction = direction; }

	/*Check if ray intersects a box
	tmin is the minimum distance along the ray (a scalar value in the range [0, infinity]) 
	where the intersection takes place*/
	bool IntersectBox(const Box &box, float &tmin) const;
	/*Check if ray intersects a rotated box*/
	bool IntersectBox(const Box &box, const glm::mat3 &orient, float &tmin) const
	{
		Ray newray((orient * (m_origin-box.GetCenter())) + box.GetCenter(), orient * m_direction);
		return newray.IntersectBox(box, tmin);
	}
	/*Check if ray intersects a 2d rectangle*/
	bool IntersectRect(const Rect &rect) const;
};

/*
A collection of a parent bounding bxo and its children
*/
struct BoundVolume
{
	Box parent;
	std::vector<Box> children;
};

/*
A plane
- consists of any point of the plane and a normal
*/
class Plane
{
private:
	glm::vec3 m_point;
	glm::vec3 m_normal;
public:
	Plane(const glm::vec3 &point = glm::vec3(0.0f), const glm::vec3 &normal = glm::vec3(0.0f)) : m_point(point), m_normal(normal) {}
	
	const glm::vec3 &GetPoint() const { return m_point; }
	const glm::vec3 &GetNormal() const { return m_normal; }
	void SetPoint(const glm::vec3 &point) { m_point = point; }
	void SetNormal(const glm::vec3 &normal) { m_normal = normal; }
	void SetNormalAndPoint(const glm::vec3 &normal, const glm::vec3 &point) { m_point = point; m_normal = normal; }

	/*Get distance from a point to this plane*/
	float GetDistance(const glm::vec3 &point) { return glm::dot(point-m_point, m_normal); }

};

/*
A frustum
- consists of 6 planes
*/
class Frustum
{
private:
	enum {
		TOP = 0, BOTTOM, LEFT,
		RIGHT, NEARP, FARP
	};
	Plane m_planes[6];

	float nearD, farD, ratio, angle, tang;
	float nw, nh, fw, fh;

	glm::vec3 PositiveVertexBox(const Box &box, const glm::vec3 &normal)
	{
		glm::vec3 p(box.GetCenter()[0] - box.GetExtents()[0], box.GetCenter()[1] - box.GetExtents()[1], box.GetCenter()[2] - box.GetExtents()[2]);
		if (normal.x >= 0)
			p.x = box.GetCenter()[0] + box.GetExtents()[0];
		if (normal.y >= 0)
			p.y = box.GetCenter()[1] + box.GetExtents()[1];
		if (normal.z >= 0)
			p.z = box.GetCenter()[2] + box.GetExtents()[2];
		return p;
	}
	/*glm::vec3 NegativeVertexBox(const Box &box, const glm::vec3 &normal)
	{
		glm::vec3 p(box.GetCenter()[0] + box.GetExtents()[0], box.GetCenter()[1] + box.GetExtents()[1], box.GetCenter()[2] + box.GetExtents()[2]);
		if (normal.x >= 0)
			p.x = box.GetCenter()[0] - box.GetExtents()[0];
		if (normal.y >= 0)
			p.y = box.GetCenter()[1] - box.GetExtents()[1];
		if (normal.z >= 0)
			p.z = box.GetCenter()[2] - box.GetExtents()[2];
		return p;
	}*/
public:
	void SetCamInternals(float angle, float ratio, float nearD, float farD) {

		// store the information
		this->ratio = ratio;
		this->angle = angle;
		this->nearD = nearD;
		this->farD = farD;

		// compute width and height of the near and far plane sections
		tang = glm::tan(glm::radians(angle * 0.5f));
		nh = nearD * tang;
		nw = nh * ratio;
		fh = farD  * tang;
		fw = fh * ratio;
	}

	void SetCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u) {

		glm::vec3 dir, nc, fc, X, Y, Z;

		Z = glm::normalize(p - l);
		X = glm::normalize(glm::cross(u, Z));
		Y = glm::cross(Z, X);

		// compute the centers of the near and far planes
		nc = p - Z * nearD;
		fc = p - Z * farD;

		m_planes[NEARP].SetNormalAndPoint(-Z, nc);
		m_planes[FARP].SetNormalAndPoint(Z, fc);

		glm::vec3 aux, normal;

		aux = glm::normalize((nc + Y*nh) - p);
		normal = glm::cross(aux, X);
		m_planes[TOP].SetNormalAndPoint(normal, nc + Y*nh);

		aux = glm::normalize((nc - Y*nh) - p);
		normal = glm::cross(X, aux);
		m_planes[BOTTOM].SetNormalAndPoint(normal, nc - Y*nh);

		aux = glm::normalize((nc - X*nw) - p);
		normal = glm::cross(aux, Y);
		m_planes[LEFT].SetNormalAndPoint(normal, nc - X*nw);

		aux = glm::normalize((nc + X*nw) - p);
		normal = glm::cross(Y, aux);
		m_planes[RIGHT].SetNormalAndPoint(normal, nc + X*nw);
	}


	/*Check if a box intersects a frustum*/
	bool BoxInFrustum(const Box &box) {
		//for each plane do ...
		for (int i = 0; i < 6; i++) {

			// is the positive vertex outside?
			if (m_planes[i].GetDistance(PositiveVertexBox(box, m_planes[i].GetNormal())) < 0)
				return false;
			// is the negative vertex outside?	
			//else if (m_planes[i].GetDistance(NegativeVertexBox(box, m_planes[i].GetNormal())) < 0)
				//return true;
		}
		return true;

	}
};

