#pragma once
#include "../graphics/stdinc.h"
#include "../scene/Unit.h"
#include <unordered_set>

//typedef std::vector<const std::unordered_set<const Unit*>*> UnitCollections;
typedef std::vector<Unit *> UnitCollections;


#define MAX_UNITS_PER_NODE 5
#define MAX_DEPTH_QUADTREE 5
class QuadTree
{
private:
	QuadTree * m_nodes;
	unsigned char m_depth;
	Rect m_rect;
	std::unordered_set<Unit*> m_units;
	typedef std::unordered_set<Unit*>::iterator UnitIterator;

public:
	QuadTree() { m_nodes = NULL; }
	~QuadTree() { Clear(); }
	
	void Initialize(unsigned char depth, const Rect &rect) {
		m_depth = depth; m_rect = rect;
	}
	
	void Split()
	{
		m_nodes = new QuadTree[4];

		float subWidth = m_rect.width / 2;
		float subHeight = m_rect.height / 2;
		float x = m_rect.x;
		float y = m_rect.y;

		m_nodes[0].Initialize(m_depth + 1, Rect(x + subWidth, y, subWidth, subHeight));
		m_nodes[1].Initialize(m_depth + 1, Rect(x, y, subWidth, subHeight));
		m_nodes[2].Initialize(m_depth + 1, Rect(x, y + subHeight, subWidth, subHeight));
		m_nodes[3].Initialize(m_depth + 1, Rect(x + subWidth, y + subHeight, subWidth, subHeight));
	}

	void Clear()
	{
		if (m_nodes){
			m_nodes[0].Clear(); m_nodes[1].Clear(); m_nodes[2].Clear(); m_nodes[3].Clear();
			delete[] m_nodes; m_nodes = NULL;
		}
		m_units.clear();
	}

	int GetIndex(Unit *unit) const
	{
		const Rect& rect = unit->GetRect();

		int index = -1;
		float verticalMidpoint = m_rect.x + (m_rect.width / 2.0f);
		float horizontalMidpoint = m_rect.y + (m_rect.height / 2.0f);

		bool topQuadrant = (rect.y < horizontalMidpoint && rect.y + rect.height < horizontalMidpoint);
		bool bottomQuadrant = (rect.y > horizontalMidpoint);

		if (rect.x < verticalMidpoint && rect.x + rect.width < verticalMidpoint) {
			if (topQuadrant) {
				index = 1;
			}
			else if (bottomQuadrant) {
				index = 2;
			}
		}
		else if (rect.x > verticalMidpoint) {
			if (topQuadrant) {
				index = 0;
			}
			else if (bottomQuadrant) {
				index = 3;
			}
		}
		return index;
	}

	void Insert(Unit* unit)
	{
		if (m_nodes) {
			int index = GetIndex(unit);

			if (index != -1)
			{
				m_nodes[index].Insert(unit);
				return;
			}
		}
		m_units.insert(unit);

		if (m_units.size() > MAX_UNITS_PER_NODE && m_depth < MAX_DEPTH_QUADTREE) {
			if (!m_nodes)
				Split();

			std::vector<UnitIterator> toerase;
			for (UnitIterator i = m_units.begin(); i != m_units.end(); ++i)
			{
				int index = GetIndex(*i);
				if (index != -1) {
					m_nodes[index].Insert(*i);
					toerase.push_back(i);
				}
			}
			for (unsigned int i = 0; i < toerase.size(); ++i) m_units.erase(toerase[i]);
		}
	}

	void Remove(Unit* unit)
	{
		if (m_nodes) {
			int index = GetIndex(unit);

			if (index != -1)
			{
				m_nodes[index].Remove(unit);
				return;
			}
		}
		
		m_units.erase(unit);
	}

	const std::unordered_set<Unit*> &GetUnits() const
	{
		return m_units;
	}

	void GetPotentialCollisions(Unit* unit, UnitCollections &unitCollections) const
	{
		if (m_nodes)
		{
			int index = GetIndex(unit);
			if (index != -1) {
				m_nodes[index].GetPotentialCollisions(unit, unitCollections);
			}
			else
			{
				m_nodes[0].GetPotentialCollisions(unit, unitCollections);
				m_nodes[1].GetPotentialCollisions(unit, unitCollections);
				m_nodes[2].GetPotentialCollisions(unit, unitCollections);
				m_nodes[3].GetPotentialCollisions(unit, unitCollections);
			}
		}
		for (UnitIterator i = m_units.begin(); i != m_units.end(); ++i)
			unitCollections.push_back(*i);
	}

	bool Intersectray(const Ray &ray)
	{
		return ray.IntersectRect(m_rect);
	}
	void GetPotentialCollisions(const Ray &ray, UnitCollections &unitCollections) const
	{
		if (m_nodes)
		{
			if (m_nodes[0].Intersectray(ray)) m_nodes[0].GetPotentialCollisions(ray, unitCollections);
			if (m_nodes[1].Intersectray(ray)) m_nodes[1].GetPotentialCollisions(ray, unitCollections);
			if (m_nodes[2].Intersectray(ray)) m_nodes[2].GetPotentialCollisions(ray, unitCollections);
			if (m_nodes[3].Intersectray(ray)) m_nodes[3].GetPotentialCollisions(ray, unitCollections);
		}

		for (UnitIterator i = m_units.begin(); i != m_units.end(); ++i)
			unitCollections.push_back(*i);
	}

};

