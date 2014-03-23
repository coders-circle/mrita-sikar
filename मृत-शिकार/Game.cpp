#include "Game.h"

void Game::Reset()
{
	m_state = GAME_PLAYING;
	m_player.SetPosition(glm::vec3(-5.0f, -45.0f, 200.0f));
	m_player.Reset();
	m_camera.Reset();
	//m_people1.Reset();

	m_scene.Reset();
	float x = 200.0f, z = -400.0f;
	
	for (unsigned i = 0; i < MAX_ZOMBIES; ++i)
	{
		m_zombies[i].SetPosition(glm::vec3(x, -45.0f, z));
		float activeness = 1.4f + 1.0f*rand() / RAND_MAX;
		m_zombies[i].SetSpeed(activeness, activeness / 1.4f);
		x += 500.0f;
		if (x >= 1300.0f){
			z += 200.0f; x = -200.0f;
		}
		m_zombies[i].Reset();
		m_scene.AddUnit(&m_zombies[i]);
	}
	m_bloodsplash.Reset();

	m_testmap.Reset();
	std::stringstream sl;
	sl << "level" << m_level << ".map";
	m_testmap.LoadMap(sl.str(), &m_scene);
	m_numpeople = m_testmap.GetPeopleCount();

	m_scene.AddUnit(&m_player);
	m_scene.AddUnit(&m_ground);
	m_scene.AddUnit(&m_blood);
	m_scene.AddUnit(&m_bloodsplash);
	m_scene.AddUnit(&m_cross);
	//m_scene.AddUnit(&m_people1);
	m_scene.AddUnit(&m_radar);

	m_deadZombies = 0;
	m_deadpeople = 0;
	
	std::stringstream ss;
	ss << "0/" << MAX_ZOMBIES << " Zombies Killed\n0/" << m_numpeople << " People Killed";
	m_scene.AddText(Text(ss.str(), 20, 680, 0.85f));
	m_scene.AddText(Text(m_player.GetPlayerHealthString(), 1000, 40));
	m_scene.AddText(Text(m_player.GetAmmoStatusString(), 1000, 680));
	m_scene.AddText(Text("Reload", 550, 220, 1.5f, false));
	m_scene.AddText(Text("LEVEL COMPLETED", 530, 220 - 60, 0.7f, false));
	m_scene.AddText(Text("GAME WON", 590, 220 - 60, 1.0f, false));
}

void Game::Initialize()
{
	m_scene.Initialize(Rect(-3000, -3000, 6000, 6000));	// quadtree needs limits of the world, send it through scene
	m_scene.SetCamera(&m_camera);

	m_bloodspr.LoadSprite("blood2.png", 512, 512, 0.0f, 0.0f, 6, 1);
	m_blood.Initialize(&m_bloodspr);

	m_bloodsplashspr.LoadSprite("bloodsplash.png", 150, 150, 0.0f, 0.0f, 3, 3);
	m_bloodsplash.Initialize(&m_bloodsplashspr);

	m_radarspr.LoadSprite("radar.png", 200, 200);
	m_radar.Initialize(&m_radarspr, glm::vec2(20, 10));
	m_radar.SetRadarRadius(1000.0f);
	m_radar.SetPlayer(&m_player);
	m_radar.SetScene(&m_scene);

	m_humanmodel.LoadModel("human.mdl");
	m_player.Initialize(&m_humanmodel, glm::vec3(-5.0f, -45.0f, 200.0f));
	m_player.SetCamera(&m_camera);
	
	//m_people1model.LoadModel("people1.mdl");
	//m_people1model.SetScale(0.28f);
	//m_people1.Initialize(&m_people1model, glm::vec3(-5.0f, -45.0f, 250.0f));

	m_testmap.Initialize(m_renderer);
	//m_testmap.Initialize("testmap.map", m_renderer, &m_scene);

	m_zombiemodel.LoadModel("zombie.mdl");
	float x = 200.0f, z = -400.0f;
	for (unsigned i = 0; i < MAX_ZOMBIES; ++i)
	{
		m_zombies[i].Initialize(&m_zombiemodel, glm::vec3(x, -45.0f, z));
	}

	m_groundmodel.LoadModel("ground.mdl");
	//m_groundmodel.SetScale(0.6f);
	m_groundmodel.SetTexture(0, "ground.jpg");
	m_ground.Initialize(&m_groundmodel, glm::vec3(0.0f, 0.0f, 0.0f));

	m_crossspr.LoadSprite("cross.png", 100.0f, 100.0f);
	m_cross.Initialize(&m_crossspr, glm::vec2(m_width / 2.0f, m_height / 2.0f));

	m_camera.Initialize(&m_player, 90.0f);

	//up vector is just opposite 
	g_audioengine->setListenerPosition(irrklang::vec3df(m_player.GetPosition().x, m_player.GetPosition().y, m_player.GetPosition().z),
		irrklang::vec3df(m_player.GetOrient()[2].x, m_player.GetOrient()[2].y, m_player.GetOrient()[2].z), irrklang::vec3df(0.0f, 0.0f, 0.0f),
		irrklang::vec3df(0.0f, -1.0f, 0.0f));
	m_player.InitAudio();
	for (unsigned i = 0; i < MAX_ZOMBIES; ++i)
	{
		m_zombies[i].InitAudio();
	}

	Reset();
}


void Game::Update(double totalTime, double deltaTime)
{
	if (m_state >= GAME_TOWIN)
	{
		m_timeend += deltaTime;
		if (m_timeend > 2)
		{
			switch (m_state)
			{
			case GAME_TOWIN: m_state = GAME_WIN; break;
			case GAME_TOLOSE: m_state = GAME_LOSE; break;
			case GAME_LEVELNEXT: ++m_level; Reset(); break;
			}
		}
	}
	if (!m_player.IsDead())
	{
		if ((m_window->CheckMButton(MOUSE_LEFT) || m_window->CheckKey(' ')) && totalTime > 3)
		{
			if (!m_justDown)
			{
				m_justDown = true;
				// the m_player.Shoot() returns true only when the timing to shoot is right (synchronizes with animation)
				if (!m_player.IsDead() && m_player.Shoot())
				{
					// Create a pickray from camera's position and direction
					glm::mat4 camInverse = glm::inverse(m_camera.GetView());
					Ray pickRay(glm::vec3(camInverse[3]), -glm::vec3(camInverse[2]));
					pickRay.SetOrigin(pickRay.GetOrigin() + pickRay.GetDirection() * m_camera.GetDistance());	// don't start ray till the distance from camera to player

					int position; float tmin;
					Unit * ClickedUnit = m_scene.GetNearestIntersection(pickRay, position, tmin, &m_player);
					// If there was a nearest intersection...
					if (ClickedUnit)
					{
						// ...and the nearest intersection was a zombie unit
						if (ClickedUnit->GetTag() == 2)
						{
							// let the zombie take a hit, returns true only if the one of the three children (0,1,2) is hit
							if (static_cast<Zombie*>(ClickedUnit)->TakeHit(position, glm::vec3(m_player.GetOrient()[2]), &m_player))
							{
								m_blood.Start(pickRay.GetOrigin() + pickRay.GetDirection() * tmin);
								if (static_cast<Zombie*>(ClickedUnit)->IsDead())
								{
									++m_deadZombies;
									std::stringstream str;
									str << m_deadZombies << "/" << MAX_ZOMBIES << " Zombies Killed\n" << m_deadpeople << "/" << m_numpeople << " People Killed";
									m_scene.ChangeText(0, str.str());	

									if (m_deadZombies == MAX_ZOMBIES)
									{
										if (m_level == m_testmap.GetNumLevels())
										{
											m_scene.ChangeText(5, "GAME WON");
											m_scene.SetTextVisible(5, true);
											m_state = GAME_TOWIN;
										}
										else
										{
											m_scene.SetTextVisible(4, true);
											m_state = GAME_LEVELNEXT;
										}
										m_timeend = 0.0;
									}
								}
							}
						}
						// if the ray intersected a people unit, kill it too
						else if (ClickedUnit->GetTag() == 10)
						{
							People * people = static_cast<People*>(ClickedUnit);
							if (!people->IsDead())	// unless it is dead
							{
								people->Die();	// kill it
								m_blood.Start(pickRay.GetOrigin() + pickRay.GetDirection() * tmin);	// show blood as well
								++m_deadpeople;

								std::stringstream str;
								str << m_deadZombies << "/" << MAX_ZOMBIES << " Zombies Killed\n" << m_deadpeople << "/" << m_numpeople << " People Killed";
								m_scene.ChangeText(0, str.str());

								if (m_deadpeople == m_numpeople)
								{
									m_scene.ChangeText(5, "GAME LOST");
									m_scene.SetTextVisible(5, true);
									m_state = GAME_TOLOSE;
								}
							}
						}
					}
				}
			}
		}
		else
			m_justDown = false;

	}

	if (m_player.GetCurrentAmmoStatus() == 0)
	{
		m_scene.SetTextVisible(3, true);
	}
	else
	{
		m_scene.SetTextVisible(3, false);
	}

	// If anything blocks player from the camera, then move camera towards the player
	// Start a ray from the PLAYER towards it's back
	Ray cameraRay(glm::vec3(m_player.GetPosition().x, 0.0f, m_player.GetPosition().z), -glm::vec3(m_player.GetOrient()[2]));
	float tmin; int position;
	// If anything intersects the ray...
	if (Unit * testunit = m_scene.GetNearestIntersection(cameraRay, position, tmin, &m_player))
	{
		// ...and the nearest intersection was less than the maximum camera distance
		// set the new distance
		if (tmin < 90.0f)
			m_camera.SetDistance(tmin);
		else
			m_camera.SetDistance(90.0f);
	}
	else
		m_camera.SetDistance(90.0f);

	if (!m_player.IsDead())
	{

		if (m_window->CheckKey('a'))	m_player.StrafeLeft();
		else m_player.EndStrafeLeft();

		if (m_window->CheckKey('d')) m_player.StrafeRight();
		else m_player.EndStrafeRight();

		if (m_window->CheckKey('w'))	m_player.Run();
		else m_player.EndRun();

		if (m_window->CheckKey('s')) m_player.BackRun();
		else m_player.EndBackRun();

		if (m_window->CheckKey('r'))
		{
			if (m_player.Reload())
				m_scene.SetTextVisible(3, false);
		}

		for (int i = 0; i < MAX_ZOMBIES; i++)
		{
			Unit * hitunit;
			if (m_zombies[i].Attacked(hitunit))
			{
				if (hitunit == &m_player)
				{
					m_player.TakeHit();
					// 8 seconds to fade
					m_bloodsplash.AddSplash(8.0f);
					if (m_player.GetHealthStatus() <= 0)
					{
						m_player.Die();
						m_camera.Die();
					}
				}
				else if (hitunit->GetTag() == 10)
				{
					if (static_cast<People*>(hitunit)->IsDead()) continue;
					static_cast<People*>(hitunit)->Die();
					glm::vec3 dir = hitunit->GetPosition() - m_zombies[i].GetPosition();
					dir.y = 0.5f;
					Ray ray(glm::vec3(m_zombies[i].GetPosition().x, 0.5f, m_zombies[i].GetPosition().z),
						glm::normalize(dir));
					float tmin;
					ray.IntersectBox(hitunit->GetBoundParent(), tmin);
					m_blood.Start(ray.GetOrigin() + ray.GetDirection() * tmin);

					++m_deadpeople;

					std::stringstream str;
					str << m_deadZombies << "/" << MAX_ZOMBIES << " Zombies Killed\n" << m_deadpeople << "/" << m_numpeople << " People Killed";
					m_scene.ChangeText(0, str.str());

					if (m_deadpeople == m_numpeople)
					{
						m_scene.ChangeText(5, "GAME LOST");
						m_scene.SetTextVisible(5, true);
						m_state = GAME_TOLOSE;
						m_timeend = 0.0;
					}
				}
			}
		}
	}

	g_audioengine->setListenerPosition(irrklang::vec3df(m_player.GetPosition().x, m_player.GetPosition().y, m_player.GetPosition().z),
		irrklang::vec3df(m_player.GetOrient()[2].x, m_player.GetOrient()[2].y, m_player.GetOrient()[2].z),
		irrklang::vec3df(0.0f, 0.0f, 0.0f), irrklang::vec3df(0.0f, -1.0f, 0.0f));

	int newx, newy;
	m_window->GetMousePos(newx, newy);
	m_player.RotateX((float)deltaTime * (newx - m_width / 2) * 2.8f);
	m_camera.RotateY((float)deltaTime * (newy - m_height / 2) * 2.8f);

	m_window->SetMousePos(m_width / 2, m_height / 2);
	m_cross.SetPosition(glm::vec2(m_width / 2 - 50, m_height / 2 - 50));
	m_scene.ChangeText(1, m_player.GetPlayerHealthString());
	m_scene.ChangeText(2, m_player.GetAmmoStatusString());
	m_scene.Update(deltaTime);
}

void Game::CleanUp()
{

	//m_people1model.CleanUp();
	m_humanmodel.CleanUp();
	m_zombiemodel.CleanUp();
	m_groundmodel.CleanUp();

	m_player.CleanUp();
	for (unsigned int i = 0; i < MAX_ZOMBIES; ++i)
		m_zombies[i].CleanUp();
	//m_people1.CleanUp();
	m_ground.CleanUp();

	m_crossspr.CleanUp();
	m_cross.CleanUp();

	m_bloodspr.CleanUp();
	m_blood.CleanUp();

	m_bloodsplashspr.CleanUp();
	m_bloodsplash.CleanUp();

	m_radarspr.CleanUp();
	m_radar.CleanUp();

	m_testmap.CleanUp();

	m_scene.CleanUp();
}