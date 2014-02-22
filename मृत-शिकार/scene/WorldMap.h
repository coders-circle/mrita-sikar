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
		while (!is.eof())
		{
			std::string mdlfilename;
			float x = 0.0f, y = 0.0f, z = 0.0f, s = 1.0f;
			is >> mdlfilename;
			is >> x >> y >> z >> s;
			Model tempmodel(renderer);
			tempmodel.LoadModel(mdlfilename);
			tempmodel.SetScale(s);
			m_worldmodels.push_back(tempmodel);
			WorldObject tempobj;
			tempobj.Initialize(&m_worldmodels[m_worldmodels.size()-1], glm::vec3(x, y, z));
			m_worldobjects.push_back(tempobj);
			m_worldobjects[m_worldmodels.size() - 1].AddToScene(scene);
		}
	}
};