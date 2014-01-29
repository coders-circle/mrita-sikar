
#include "graphics/graphics.h"
#include "scene/Scene.h"
#include "scene/Player.h"
#include "scene/Zombie.h"
#include "scene/Ground.h"
#include "scene/TPCamera.h"
#include "scene/Unit2d.h"

Window g_window;
Renderer g_renderer(&g_window);

int g_width = 900;
int g_height = 650;


Scene g_scene(&g_renderer);
TPCamera g_camera;
Model g_humanmodel(&g_renderer), g_zombiemodel(&g_renderer);
Player g_player;
Zombie g_zombie;

Sprite g_crossspr(&g_renderer);
Unit2d g_cross;

Model g_groundmodel(&g_renderer);
Ground g_ground;

//Model g_testmodel(&g_renderer);
//Ground g_test;

void Initialize()
{
	g_renderer.Initialize();
	g_scene.Initialize();
	g_scene.SetCamera(&g_camera);

	g_humanmodel.LoadModel("human.mdl");
	g_player.Initialize(&g_humanmodel, glm::vec3(-5.0f, -45.0f, -70.0f));

	g_zombiemodel.LoadModel("zombie.mdl");
	g_zombie.Initialize(&g_zombiemodel, glm::vec3(5.0f, -45.0f, -150.0f));

	Mesh mesh(&g_renderer);
	Mesh::CreateBox(&mesh, glm::vec3(10000.0f, 0.5f, 10000.0f));
	g_groundmodel.AddMesh(mesh);
	g_groundmodel.SetTexture(0, "diffuse_1.jpg");
	g_ground.Initialize(&g_groundmodel, glm::vec3(0.0f, -45.0f - 0.25f, -50.0f));

	/*
	Mesh tmesh(&g_renderer);
	Mesh::CreateBox(&tmesh, glm::vec3(g_zombie.GetBoundVolume().m_parent.Extents()));
	g_testmodel.AddMesh(tmesh);
	g_testmodel.SetTexture(0, "diffuse_1.jpg");
	g_test.Initialize(&g_testmodel, glm::vec3(5.0f, -45.0f, -150.0f) + g_zombie.GetBoundVolume().m_parent.Center);
	*/

	g_crossspr.LoadSprite("cross.png", 32.0f, 32.0f);//, 5.0f, 5.0f);
	g_cross.Initialize(&g_crossspr, glm::vec2(g_width/2.0f, g_height/2.0f));

	g_scene.AddUnit(&g_player);
	g_scene.AddUnit(&g_zombie);
	g_scene.AddUnit(&g_ground);
	g_scene.AddUnit(&g_cross);

	//g_scene.AddUnit(&g_test);

	g_camera.SetParameters(&g_player, 70.0f);

	g_window.SetMousePos(g_width / 2, g_height / 2);
	g_window.ShowMouseCursor(false);
}

void CleanUp()
{
	g_humanmodel.CleanUp();
	g_zombiemodel.CleanUp();
	g_groundmodel.CleanUp();

	g_player.CleanUp();
	g_zombie.CleanUp();
	g_ground.CleanUp();

	g_crossspr.CleanUp();
	g_cross.CleanUp();

	//g_testmodel.CleanUp();
	//g_test.CleanUp();

	g_renderer.CleanUp();
	g_scene.CleanUp();
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
		}
	}
	else
		g_justDown = false;

	if (g_window.CheckKey('a')) g_player.StrafeLeft();
	else g_player.EndStrafeLeft();

	if (g_window.CheckKey('d')) g_player.StrafeRight();
	else g_player.EndStrafeRight();

	if (g_window.CheckKey('w')) g_player.Run();
	else g_player.EndRun();

	if (g_window.CheckKey('s')) g_player.BackRun();
	else g_player.EndBackRun();

	int newx, newy;
	g_window.GetMousePos(newx, newy);
	g_player.RotateX((float)deltaTime * (newx - g_width/2) * 2.8f);
	g_player.RotateY((float)deltaTime * (newy - g_height/2) * 2.8f);
	
	g_window.SetMousePos(g_width / 2, g_height / 2);

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