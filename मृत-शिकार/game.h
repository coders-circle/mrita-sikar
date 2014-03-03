#include "graphics/graphics.h"

#include "scene/Scene.h"
#include "scene/Player.h"
#include "scene/Zombie.h"
#include "scene/Ground.h"
#include "scene/TPCamera.h"
#include "scene/Unit2d.h"
#include "scene/WorldMap.h"

#include "audio/audio.h"

#include "glm/gtx/vector_angle.hpp"

Window g_window;
Renderer g_renderer(&g_window);


int g_width = 900;
int g_height = 650;


Scene g_scene(&g_renderer);
TPCamera g_camera;
Model g_humanmodel(&g_renderer), g_zombiemodel(&g_renderer);
Player g_player;
WorldMap g_testmap;


#define MAX_ZOMBIES 2
Zombie g_zombies[MAX_ZOMBIES];

Sprite g_crossspr(&g_renderer);
Unit2d g_cross;

Model g_groundmodel(&g_renderer);
Ground g_ground;

irrklang::ISoundEngine* g_audioengine = 0;

void Initialize()
{
	g_renderer.Initialize();
	g_scene.Initialize(Rect(-5000, -5000, 10000, 10000));	// quadtree needs limits of the world
	g_scene.SetCamera(&g_camera);

	g_humanmodel.LoadModel("human.mdl");
	g_player.Initialize(&g_humanmodel, glm::vec3(-5.0f, -45.0f, 200.0f));
	g_testmap.Initialize("testmap.map", &g_renderer, &g_scene);

	g_zombiemodel.LoadModel("zombie.mdl");
	float x = 200.0f, z= -400.0f;
	for (unsigned i = 0; i < MAX_ZOMBIES; ++i)
	{
		g_zombies[i].Initialize(&g_zombiemodel, glm::vec3(x, -45.0f, z));
		x += 500.0f; 
		if (x >= 1300.0f){
			z += 200.0f; x = -200.0f;
		}
		g_scene.AddUnit(&g_zombies[i]);
	}
	
	Mesh mesh(&g_renderer);
	Mesh::CreateBox(&mesh, glm::vec3(1000.0f, 0.5f, 1000.0f));
	g_groundmodel.AddMesh(mesh);
	g_groundmodel.SetBoundBox(Box(glm::vec3(0.0f), glm::vec3(1000.0f, 0.5f, 1000.0f)));
	g_groundmodel.SetTexture(0, "ground.jpg");
	g_ground.Initialize(&g_groundmodel, glm::vec3(0.0f, -45.0f - 0.25f, -50.0f));
	g_crossspr.LoadSprite("cross.png", 100.0f, 100.0f);//, 5.0f, 5.0f);
	g_cross.Initialize(&g_crossspr, glm::vec2(g_width/2.0f, g_height/2.0f));
	g_scene.AddUnit(&g_player);
	g_scene.AddUnit(&g_ground);
	g_scene.AddUnit(&g_cross);
	g_camera.Initialize(&g_player, 110.0f);
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
}

void CleanUp()
{
	g_humanmodel.CleanUp();
	g_zombiemodel.CleanUp();
	g_groundmodel.CleanUp();
//	g_housemodel.CleanUp();


	g_player.CleanUp();
	for (unsigned int i = 0; i < MAX_ZOMBIES; ++i)
		g_zombies[i].CleanUp();
	g_ground.CleanUp();

	g_crossspr.CleanUp();
	g_cross.CleanUp();


	g_renderer.CleanUp();
	g_scene.CleanUp();

}

std::ostream & operator << (std::ostream & os, const glm::vec3&v)
{
	os << v.x << " " << v.y << " " << v.z;
	return os;
}
bool g_justDown = false;
void Update(double totalTime, double deltaTime)
{
	if (g_window.CheckMButton(MOUSE_LEFT) || g_window.CheckKey(' '))
	{
		if (!g_justDown)
		{
			g_justDown = true;
			g_player.Shoot();


			int mx, my;
			g_window.GetMousePos(mx, my);

			glm::mat4 camInverse = glm::inverse(g_camera.GetView());
			Ray pickRay(glm::vec3(camInverse[3]), -glm::vec3(camInverse[2]));

			Unit * ClickedUnit = g_scene.GetNearestIntersection(pickRay, &g_player);
			if (ClickedUnit)
			{
				if (ClickedUnit->GetTag() == 2)
				{
					// May be check for ray against children boxes here
					//  pickRay.Intersect(ClickedUnit->GetChildBox(xxx), glm::mat3(ClickedUnit->GetOrient()));
					//  xxx = 
					//	0 for head
					//  1 for chest
					//	2 for bottom
					// (Note that there is another child box (3) that can be checked against the player to see if the attack
					// is successfull when the zombie is in ZOMBIE_ATTACK mode)
					//
					// For now just die
					static_cast<Zombie*>(ClickedUnit)->Die();
				}
			}
		}
	}
	else
		g_justDown = false;

	if (g_window.CheckKey('a'))	g_player.StrafeLeft();
	else g_player.EndStrafeLeft();

	if (g_window.CheckKey('d')) g_player.StrafeRight();
	else g_player.EndStrafeRight();

	if (g_window.CheckKey('w'))	g_player.Run();
	else g_player.EndRun();

	if (g_window.CheckKey('s')) g_player.BackRun();
	else g_player.EndBackRun();

	for (int i = 0; i < MAX_ZOMBIES; i++)
	{
		g_zombies[i].SetDestination(g_player.GetBoundCenter());
	}

	g_audioengine->setListenerPosition(irrklang::vec3df(g_player.GetPosition().x, g_player.GetPosition().y, g_player.GetPosition().z), 
		irrklang::vec3df(g_player.GetOrient()[2].x, g_player.GetOrient()[2].y, g_player.GetOrient()[2].z),
		irrklang::vec3df(0.0f, 0.0f, 0.0f), irrklang::vec3df(0.0f, -1.0f, 0.0f));

	int newx, newy;
	g_window.GetMousePos(newx, newy);
	g_player.RotateX((float)deltaTime * (newx - g_width / 2) * 2.8f);
	g_camera.RotateX((float)deltaTime * (newx - g_width / 2) * 2.8f);
	g_camera.RotateY((float)deltaTime * (newy - g_height / 2) * 2.8f);

	g_window.SetMousePos(g_width / 2, g_height / 2);
	g_cross.SetPosition(glm::vec2(g_width/2 - 50, g_height/2 - 50));

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