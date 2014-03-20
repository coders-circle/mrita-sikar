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

#include "audio/audio.h"

#include "glm/gtx/vector_angle.hpp"

#include <sstream>

Window g_window;
Renderer g_renderer(&g_window);


int g_width = 900;
int g_height = 650;


GameScene g_scene(&g_renderer);
TPCamera g_camera;
Model g_humanmodel(&g_renderer), g_zombiemodel(&g_renderer);
Player g_player;
WorldMap g_testmap;

Sprite g_bloodspr(&g_renderer);
Blood g_blood;

Sprite g_bloodsplashspr(&g_renderer);
BloodSplash g_bloodsplash;

/*Sprite g_bigbloodspr(&g_renderer);
Unit2d g_bigblood;*/

Sprite g_radarspr(&g_renderer);
Radar g_radar;

#define MAX_ZOMBIES 5
Zombie g_zombies[MAX_ZOMBIES];

Sprite g_crossspr(&g_renderer);
Unit2d g_cross;

Model g_groundmodel(&g_renderer);
Ground g_ground;

Model g_people1model(&g_renderer);
People g_people1;

irrklang::ISoundEngine* g_audioengine = 0;

void Initialize()
{
	g_renderer.Initialize();
	g_scene.Initialize(Rect(-5000, -5000, 10000, 10000));	// quadtree needs limits of the world, send it through scene
	g_scene.SetCamera(&g_camera);
	
	g_bloodspr.LoadSprite("blood2.png", 128, 128, 0.0f, 0.0f, 6, 1);
	g_blood.Initialize(&g_bloodspr);

	g_bloodsplashspr.LoadSprite("bloodsplash.png", 150, 150, 0.0f, 0.0f, 3, 3);
	g_bloodsplash.Initialize(&g_bloodsplashspr);

	g_radarspr.LoadSprite("radar.png", 200, 200);
	g_radar.Initialize(&g_radarspr, glm::vec2(20, 10));
	g_radar.SetRadarRadius(1000.0f);
	g_radar.SetPlayer(&g_player);
	g_radar.SetScene(&g_scene);

	/*g_bigbloodspr.LoadSprite("bloodsplash_big.png", 600, 600);
	g_bigblood.Initialize(&g_bigbloodspr, glm::vec2(g_width / 2.0f - 600 / 2.0f, g_height / 2.0f - 600 / 2.0f + 100.0f));
	g_bigblood.SetVisible(false);*/

	g_humanmodel.LoadModel("human.mdl");
	g_player.Initialize(&g_humanmodel, glm::vec3(-5.0f, -45.0f, 200.0f));
	g_player.SetCamera(&g_camera);
	g_testmap.Initialize("testmap.map", &g_renderer, &g_scene);

	g_people1model.LoadModel("people1.mdl");
	g_people1model.SetScale(0.28f);
	g_people1.Initialize(&g_people1model, glm::vec3(-5.0f, -45.0f, 250.0f));

	g_zombiemodel.LoadModel("zombie.mdl");
	float x = 200.0f, z= -400.0f;
	for (unsigned i = 0; i < MAX_ZOMBIES; ++i)
	{
		g_zombies[i].Initialize(&g_zombiemodel, glm::vec3(x, -45.0f, z));
		float activeness = 1.4f + 1.0f*rand() / RAND_MAX;
		g_zombies[i].SetSpeed(activeness, activeness/1.4f);
		x += 500.0f; 
		if (x >= 1300.0f){
			z += 200.0f; x = -200.0f;
		}
		g_scene.AddUnit(&g_zombies[i]);
	}
	
	g_groundmodel.LoadModel("ground.mdl");
	g_groundmodel.SetTexture(0, "ground.jpg");
	g_ground.Initialize(&g_groundmodel, glm::vec3(0.0f, 0.0f, 0.0f));

	g_crossspr.LoadSprite("cross.png", 100.0f, 100.0f);
	g_cross.Initialize(&g_crossspr, glm::vec2(g_width/2.0f, g_height/2.0f));
	g_scene.AddUnit(&g_player);
	g_scene.AddUnit(&g_ground);
	g_scene.AddUnit(&g_blood);
	g_scene.AddUnit(&g_bloodsplash);
	g_scene.AddUnit(&g_cross);
	g_scene.AddUnit(&g_people1);
	g_scene.AddUnit(&g_radar);
	//g_scene.AddUnit(&g_bigblood);

	g_camera.Initialize(&g_player, 90.0f);
	g_window.SetMousePos(g_width / 2, g_height / 2);
	g_window.ShowMouseCursor(false);

	g_audioengine = irrklang::createIrrKlangDevice(); 
	//up vector is just opposite 
	g_audioengine->setListenerPosition(irrklang::vec3df(g_player.GetPosition().x, g_player.GetPosition().y, g_player.GetPosition().z),
		irrklang::vec3df(g_player.GetOrient()[2].x, g_player.GetOrient()[2].y, g_player.GetOrient()[2].z), irrklang::vec3df(0.0f, 0.0f, 0.0f), 
		irrklang::vec3df(0.0f, -1.0f, 0.0f));
	g_player.InitAudio();
	for (unsigned i = 0; i < MAX_ZOMBIES; ++i)
	{
		g_zombies[i].InitAudio();
	}

	g_scene.AddText(Text("Zombies Killed: 0", 20, 680, 0.85f));
	g_scene.AddText(Text(g_player.GetPlayerHealthString(), 1000, 40));
	g_scene.AddText(Text(g_player.GetAmmoStatusString(), 1000, 680));
}

void CleanUp()
{
	g_people1model.CleanUp();
	g_humanmodel.CleanUp();
	g_zombiemodel.CleanUp();
	g_groundmodel.CleanUp();
	
	g_player.CleanUp();
	for (unsigned int i = 0; i < MAX_ZOMBIES; ++i)
		g_zombies[i].CleanUp();
	g_people1.CleanUp();
	g_ground.CleanUp();

	g_crossspr.CleanUp();
	g_cross.CleanUp();

	g_bloodspr.CleanUp();
	g_blood.CleanUp();

	g_bloodsplashspr.CleanUp();
	g_bloodsplash.CleanUp();

	/*g_bigbloodspr.CleanUp();
	g_bigblood.CleanUp();*/
	
	g_radarspr.CleanUp();
	g_radar.CleanUp();

	g_renderer.CleanUp();
	g_scene.CleanUp();
}

std::ostream & operator << (std::ostream & os, const glm::vec3&v)
{
	os << v.x << " " << v.y << " " << v.z;
	return os;
}


bool g_justDown = false;
int g_deadZombies = 0;	
void Update(double totalTime, double deltaTime)
{
	if (!g_player.IsDead())
	{
		if (g_window.CheckMButton(MOUSE_LEFT) || g_window.CheckKey(' '))
		{
			if (!g_justDown)
			{
				g_justDown = true;
				// the g_player.Shoot() returns true only when the timing to shoot is right (synchronizes with animation)
				if (!g_player.IsDead() && g_player.Shoot())
				{
					// Create a pickray from camera's position and direction
					glm::mat4 camInverse = glm::inverse(g_camera.GetView());
					Ray pickRay(glm::vec3(camInverse[3]), -glm::vec3(camInverse[2]));
					pickRay.SetOrigin(pickRay.GetOrigin() + pickRay.GetDirection() * g_camera.GetDistance());	// don't start ray till the distance from camera to player

					int position; float tmin;
					Unit * ClickedUnit = g_scene.GetNearestIntersection(pickRay, position, tmin, &g_player);
					// If there was a nearest intersection...
					if (ClickedUnit)
					{
						// ...and the nearest intersection was a zombie unit
						if (ClickedUnit->GetTag() == 2)
						{
							// let the zombie take a hit, returns true only if the one of the three children (0,1,2) is hit
							if (static_cast<Zombie*>(ClickedUnit)->TakeHit(position, glm::vec3(g_player.GetOrient()[2])))
							{
								g_blood.Start(pickRay.GetOrigin() + pickRay.GetDirection() * tmin);
								if (static_cast<Zombie*>(ClickedUnit)->IsDead())
								{
									++g_deadZombies;
									std::stringstream str;
									str << "Dead Zombies: " << g_deadZombies;
									g_scene.ChangeText(0, str.str());	// 0 is the index of the only text added
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
								g_blood.Start(pickRay.GetOrigin() + pickRay.GetDirection() * tmin);	// show blood as well
							}
						}
					}
				}
			}
		}
		else
			g_justDown = false;

	}

	// If anything blocks player from the camera, then move camera towards the player
	// Start a ray from the PLAYER towards it's back
	Ray cameraRay(glm::vec3(g_player.GetPosition().x, 0.0f, g_player.GetPosition().z), -glm::vec3(g_player.GetOrient()[2]));
	float tmin; int position;
	// If anything intersects the ray...
	if (Unit * testunit = g_scene.GetNearestIntersection(cameraRay, position, tmin, &g_player))
	{
		// ...and the nearest intersection was less than the maximum camera distance
		// set the new distance
		if (tmin < 90.0f)
			g_camera.SetDistance(tmin);
		else
			g_camera.SetDistance(90.0f);
	}
	else
		g_camera.SetDistance(90.0f);
	
	if (!g_player.IsDead())
	{
		if (g_window.CheckKey('a'))	g_player.StrafeLeft();
		else g_player.EndStrafeLeft();

		if (g_window.CheckKey('d')) g_player.StrafeRight();
		else g_player.EndStrafeRight();

		if (g_window.CheckKey('w'))	g_player.Run();
		else g_player.EndRun();

		if (g_window.CheckKey('s')) g_player.BackRun();
		else g_player.EndBackRun();

		if (g_window.CheckKey('r')) g_player.Reload();

		for (int i = 0; i < MAX_ZOMBIES; i++)
		{
			g_zombies[i].SetDestination(g_player.GetBoundCenter());
			if (g_zombies[i].Attacked())
			{
				g_player.TakeHit();
				// 8 seconds to fade
				g_bloodsplash.AddSplash(8.0f);
				if (g_player.GetHealthStatus() <= 0)
				{
					g_player.Die();
					g_camera.Die();
					//g_bigblood.SetVisible(true);
				}
			}
		}
	}

	g_audioengine->setListenerPosition(irrklang::vec3df(g_player.GetPosition().x, g_player.GetPosition().y, g_player.GetPosition().z), 
		irrklang::vec3df(g_player.GetOrient()[2].x, g_player.GetOrient()[2].y, g_player.GetOrient()[2].z),
		irrklang::vec3df(0.0f, 0.0f, 0.0f), irrklang::vec3df(0.0f, -1.0f, 0.0f));

	int newx, newy;
	g_window.GetMousePos(newx, newy);
	g_player.RotateX((float)deltaTime * (newx - g_width / 2) * 2.8f);
	g_camera.RotateY((float)deltaTime * (newy - g_height / 2) * 2.8f);

	g_window.SetMousePos(g_width / 2, g_height / 2);
	g_cross.SetPosition(glm::vec2(g_width/2 - 50, g_height/2 - 50));
	g_scene.ChangeText(1, g_player.GetPlayerHealthString());
	g_scene.ChangeText(2, g_player.GetAmmoStatusString());
	g_scene.Update(deltaTime);
}

void Render()
{
	g_scene.Draw();
}

void Resize(int width, int height)
{
	g_width = width; g_height = height;
	g_scene.Resize(static_cast<float>(width), static_cast<float>(height));
}