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
		m_worldobjects.resize(n);

		for (int i = 0; i < n; i++)
		{
			std::string mdlfilename;
			float x = 0.0f, y = 0.0f, z = 0.0f, s = 1.0f;
			is >> mdlfilename;
			is >> x >> y >> z >> s;
			m_worldmodels[i].SetRenderer(renderer);
			m_worldmodels[i].LoadModel(mdlfilename);
			m_worldmodels[i].SetScale(s);
			m_worldobjects[i].Initialize(&m_worldmodels[i], glm::vec3(x, y, z));
			m_worldobjects[i].AddToScene(scene);
		}
		is.close();

		m_worldobjects[0].RotateY(90.0f);

		//while (!is.eof())
		//{
		//	float x = 0.0f, y = 0.0f, z = 0.0f, s = 1.0f;
		//	is >> mdlfilename;
		//	is >> x >> y >> z >> s;
		//	Model tempmodel(renderer);
		//	tempmodel.LoadModel(mdlfilename);
		//	tempmodel.SetScale(s);
		//	m_worldmodels.push_back(tempmodel);
		//	WorldObject tempobj;
		//	tempobj.Initialize(&m_worldmodels[m_worldmodels.size()-1], glm::vec3(x, y, z));
		//	m_worldobjects.push_back(tempobj);
		//	m_worldobjects[m_worldmodels.size() - 1].AddToScene(scene);
		//	//tempmodel.CleanUp();
		//	//tempobj.CleanUp();
		//}
		//
	}
};