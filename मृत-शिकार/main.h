#include "Game.h"

enum GlobalGameStates {MAINMENU, GAME};

Window g_window;
Renderer g_renderer(&g_window);

int g_globalState = MAINMENU;

int g_width = 900;
int g_height = 650;


Scene g_menuscene(&g_renderer);
Scene g_loadingscene(&g_renderer);
Menu g_menu;
Game g_game(&g_renderer, &g_window);

Sprite g_menubackspr(&g_renderer);
Unit2d g_menuback;

Sprite g_splashscreen(&g_renderer);
Unit2d g_splash;

irrklang::ISoundEngine* g_audioengine = 0;

#include <process.h>

volatile bool loadingFinished = false;

void AsyncLoader(void* pData)
{
	g_game.Initialize();
	loadingFinished = true;
}

void Initialize()
{
	g_renderer.Initialize();

	g_menuscene.Initialize();
	g_menu.SetScene(&g_menuscene);
	
	g_menubackspr.LoadSprite("menuback.jpg", g_width, g_height);
	g_menuback.Initialize(&g_menubackspr);;
	g_menuscene.AddUnit(&g_menuback);

	std::string items[] = { "CONTINUE GAME", "NEW GAME", "CREDITS", "EXIT" };
	for (int i = 0; i < 4; ++i)
		g_menu.AddItem(items[i], g_width / 2.0f - (items[i].length()*11.4f), 240 + 60.0f*i);


	g_menu.EnableItem(0, false);
	g_loadingscene.Initialize();
	g_splashscreen.LoadSprite("Splash.jpg", g_width, g_height);
	g_splash.Initialize(&g_splashscreen);
	g_audioengine = irrklang::createIrrKlangDevice();

	g_loadingscene.Resize(g_width, g_height);
	g_loadingscene.Update(0.1f);
	g_loadingscene.AddUnit(&g_splash);

	g_loadingscene.Draw();
	g_game.Initialize();
	//_beginthread(AsyncLoader, 500, 0);
}

void CleanUp()
{
	g_game.CleanUp();

	g_menubackspr.CleanUp();
	g_menuback.CleanUp();

	g_splashscreen.CleanUp();
	g_splash.CleanUp();

	g_menuscene.CleanUp();
	g_loadingscene.CleanUp();
	g_renderer.CleanUp();
}

std::ostream & operator << (std::ostream & os, const glm::vec3&v)
{
	os << v.x << " " << v.y << " " << v.z;
	return os;
}

void Resize(int width, int height)
{
	g_width = width; g_height = height;
	if (g_globalState == MAINMENU)
		g_menuscene.Resize(static_cast<float>(width), static_cast<float>(height));
	else if (g_globalState == GAME)
		g_game.Resize(width, height);
}

void Update(double totalTime, double deltaTime)
{
	
	if (g_globalState == MAINMENU)
	{
		int mx, my;
		g_window.GetMousePos(mx, my);
		int menuitem = g_menu.Check(static_cast<float>(mx), static_cast<float>(my), g_window.CheckMButton(MOUSE_LEFT));
		switch (menuitem)
		{
		case 1:
			g_game.SetLevel(1);
			g_game.Reset();
		case 0:
			g_globalState = GAME;
			g_window.SetMousePos(g_width / 2, g_height / 2);
			Resize(g_width, g_height);
			g_window.ResetTimer();
			g_window.ShowMouseCursor(false);
			break;
		case 3:
			g_window.Quit();
			break;
		}
		g_menuscene.Update(deltaTime);
	}
	else if (g_globalState == GAME)
	{
		g_game.Update(totalTime, deltaTime);
		if ((g_game.GetState() == Game::GAME_WIN || g_game.GetState() == Game::GAME_LOSE)
			|| (g_window.CheckKey(VK_ESCAPE)))
		{
			g_globalState = MAINMENU;
			Resize(g_width, g_height);
			g_window.ResetTimer();
			g_window.ShowMouseCursor(true);
			g_menu.EnableItem(0, true);
		}
	}
}

void Render()
{
	if (g_globalState == MAINMENU)
		g_menuscene.Draw();
	else if (g_globalState == GAME)
		g_game.Render();
}
