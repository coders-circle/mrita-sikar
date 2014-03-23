#pragma once

#include "WorldObject.h"

#include <fstream>
#include <sstream>

class WorldMap
{
private:
	std::vector<WorldObject> m_worldobjects;
	std::vector<Model> m_worldmodels;

	int m_numLevels;

	bool FileExist(std::string filename)
	{
		std::ifstream infile(filename);
		return infile.good();
	}
public:
	WorldMap()
	{
	}
	void Initialize(Renderer* renderer)
	{
		m_worldmodels.resize(10);
		for (unsigned int i = 0; i < m_worldmodels.size(); ++i)
			m_worldmodels[i].SetRenderer(renderer);

		m_worldmodels[0].LoadModel("building1.mdl");
		m_worldmodels[1].LoadModel("building2.mdl");
		m_worldmodels[2].LoadModel("building12.mdl");
		m_worldmodels[3].LoadModel("building13.mdl");
		m_worldmodels[4].LoadModel("building3.mdl");
		m_worldmodels[5].LoadModel("building5.mdl");
		m_worldmodels[6].LoadModel("building6.mdl");
		m_worldmodels[7].LoadModel("building7.mdl");
		m_worldmodels[8].LoadModel("building8.mdl");
		m_worldmodels[9].LoadModel("building9.mdl");
		for (int i = 0; i < 10; ++i)
			m_worldmodels[i].SetScale(0.3f);

		int i = 0;
		while (true)
		{
			std::stringstream sl;
			sl << "levels\\level" << i+1 << ".map";
			if (!FileExist(sl.str())) break;
			++i;
		}
		m_numLevels = i;
		std::cout << "Level count: " << m_numLevels;
	}

	void LoadMap(std::string filename, GameScene * scene)
	{
		std::ifstream is;
		is.open("levels\\"+filename, std::ios::in);
		if (is.fail())
		{
			std::cout << "Error reading map file: " << filename;
			return;
		}
		int n = 0;
		is >> n;
		m_worldobjects.resize(n);

		for (int i = 0; i < n; i++)
		{
			int index = 0;
			is >> index;
			if (index >= 0 && index < static_cast<int>(m_worldmodels.size()))
			{
				float fx = 0.0f, fy = 0.0f, fz = 0.0f, ry = 0.0f;
				is >> fx >> fy >> fz >> ry;
				m_worldobjects[i].Initialize(&m_worldmodels[index], glm::vec3(fx, fy, fz));
				m_worldobjects[i].AddToScene(scene);
				m_worldobjects[i].RotateY(ry);

			}
			else
			{
				std::cout << "Error parsing map";
				break;
			}
		}
		is.close();

	}

	void Reset()
	{
		for (unsigned i = 0; i < m_worldobjects.size(); ++i)
			m_worldobjects[i].CleanUp();
		m_worldobjects.clear();
	}

	void CleanUp()
	{
		Reset();
		for (unsigned int i = 0; i < m_worldmodels.size(); ++i)
			m_worldmodels[i].CleanUp();
		m_worldmodels.clear();
	}

	int GetNumLevels()
	{
		return m_numLevels;
	}
};