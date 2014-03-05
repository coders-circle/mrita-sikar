#pragma once

#include "WorldObject.h"


#include <fstream>

class WorldMap
{
private:
	std::vector<WorldObject> m_worldobjects;
	std::vector<Model> m_worldmodels;
public:
	WorldMap()
	{
	}
	void Initialize(std::string mapfilename, Renderer* renderer, Scene* scene)
	{
		
		std::ifstream is;
		is.open(mapfilename, std::ios::in);
		if (is.fail())
		{
			std::cout << "Error reading map file: " << mapfilename;
			return;
		}
		int n = 0;
		is >> n;

		m_worldmodels.resize(n);
		for (int i = 0; i < n; i++)
		{
			std::string mdlfilename;
			is >> mdlfilename;
			float scale = 1.0f;
			is >> scale;
			m_worldmodels[i].SetRenderer(renderer);
			m_worldmodels[i].LoadModel(mdlfilename);
			m_worldmodels[i].SetScale(scale);
		}

		is >> n;
		m_worldobjects.resize(n);

		for (int i = 0; i < n; i++)
		{
			int index = 0;
			is >> index;
			if (index >= 0 && index < m_worldmodels.size())
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
};