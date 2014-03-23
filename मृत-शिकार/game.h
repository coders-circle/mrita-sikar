#pragma once
#include "graphics/graphics.h"

#include "scene/Player.h"
#include "scene/Zombie.h"
#include "scene/Ground.h"
#include "scene/TPCamera.h"
#include "scene/Unit2d.h"
#include "scene/WorldMap.h"
#include "scene/Blood.h"
#include "scene/BloodSplash.h"
#include "scene/People.h"
#include "scene/Radar.h"
#include "scene/UI.h"

#include "audio/audio.h"

#include "glm/gtx/vector_angle.hpp"

#include <sstream>

class Game
{
private:
	Renderer * m_renderer;
	Window * m_window;
	int m_width, m_height;

	GameScene m_scene;
	TPCamera m_camera;
	Model m_humanmodel, m_zombiemodel;
	Player m_player;
	WorldMap m_testmap;

	Sprite m_bloodspr;
	Blood m_blood;

	Sprite m_bloodsplashspr;
	BloodSplash m_bloodsplash;

	Sprite m_radarspr;
	Radar m_radar;

#define MAX_ZOMBIES 25
	Zombie m_zombies[MAX_ZOMBIES];

	Sprite m_crossspr;
	Unit2d m_cross;

	Model m_groundmodel;
	Ground m_ground;

	Model m_people1model;
	People m_people1;
	int m_numpeople;
	int m_deadpeople;

	bool m_justDown;
	int m_deadZombies;
	
	int m_level;
	int m_state;
	double m_timeend;
public:
	enum GameState{ GAME_WIN, GAME_LOSE, GAME_PLAYING, GAME_TOWIN, GAME_TOLOSE, GAME_LEVELNEXT };
	Game(Renderer * renderer, Window * window) :m_renderer(renderer), m_window(window),
		m_scene(renderer), m_humanmodel(renderer), m_zombiemodel(renderer),
		m_bloodspr(renderer), m_bloodsplashspr(renderer), m_radarspr(renderer),
		m_crossspr(renderer), m_groundmodel(renderer), m_people1model(renderer),
		m_justDown(false), m_deadZombies(0), m_state(GAME_PLAYING), m_level(1), m_numpeople(1), m_deadpeople(0)
	{
	}

	void Initialize();
	void Reset();
	void Update(double totalTime, double deltaTime);
	void Resize(int width, int height)
	{
		m_width = width; m_height = height;
		m_scene.Resize(static_cast<float>(width), static_cast<float>(height));
	}
	void Render() { m_scene.Draw(); }
	void CleanUp();

	int GetState() { return m_state; }
	void SetLevel(int level) { m_level = level; }
};

