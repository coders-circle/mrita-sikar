#pragma once
#include "../graphics/stdinc.h"
#include "../scene/Unit.h"

typedef std::vector<const std::vector<const Unit*>*> UnitCollections;

#define MAX_UNITS_PER_NODE 10
#define MAX_DEPTH_QUADTREE 5
class QuadTree
{
private:
	QuadTree * m_nodes;
	unsigned char m_depth;
	Rect m_rect;
	std::vector<const Unit*> m_units;

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

	int GetIndex(const Unit *unit) const
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

	void Insert(const Unit* unit)
	{
		if (m_nodes) {
			int index = GetIndex(unit);

			if (index != -1)
			{
				m_nodes[index].Insert(unit);
				return;
			}
		}
		m_units.push_back(unit);

		if (m_units.size() > MAX_UNITS_PER_NODE && m_depth < MAX_DEPTH_QUADTREE) {
			if (!m_nodes)
				Split();

			for (unsigned int i = 0; i < m_units.size(); ++i)
			{
				int index = GetIndex(m_units[i]);
				if (index != -1) {
					m_nodes[index].Insert(m_units[i]);
					m_units.erase(m_units.begin() + i);
				}
				else {
					i++;
				}
			}
		}
	}

	void Remove(const Unit* unit)
	{
		if (m_nodes) {
			int index = GetIndex(unit);

			if (index != -1)
			{
				m_nodes[index].Remove(unit);
				return;
			}
		}
		for (unsigned i = 0; i < m_units.size(); ++i)
		if (m_units[i] == unit) { m_units.erase(m_units.begin() + i); break; }
	}

	const std::vector<const Unit*> &GetUnits() const
	{ return m_units; }

	void GetPotentialCollisions(const Unit* unit, UnitCollections &unitCollections) const
	{
		int index = GetIndex(unit);
		if (index != -1 && m_nodes) {
			m_nodes[index].GetPotentialCollisions(unit, unitCollections);
		}
		unitCollections.push_back(&m_units);
	}

};

