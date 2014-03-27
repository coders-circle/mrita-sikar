#include "Game.h"

enum GlobalGameStates {MAINMENU, GAME, SPLASH, CREDITS};

Window g_window;
Renderer g_renderer(&g_window);

int g_globalState = SPLASH;

int g_width = 900;
int g_height = 650;


Scene g_menuscene(&g_renderer);
Scene g_loadingscene(&g_renderer);
Menu g_menu;
Game g_game(&g_renderer, &g_window);

Scene g_creditscene(&g_renderer);
Sprite g_creditspr(&g_renderer);
Unit2d g_credit;

Sprite g_menubackspr(&g_renderer);
Unit2d g_menuback;

Sprite g_splashscreen(&g_renderer);
Unit2d g_splash;

irrklang::ISoundEngine* g_audioengine = 0;


void Initialize()
{
	g_renderer.Initialize();

	g_menuscene.Initialize();
	g_menu.SetScene(&g_menuscene);
	
	g_menubackspr.LoadSprite("menuback.jpg", static_cast<float>(g_width), static_cast<float>(g_height));
	g_menuback.Initialize(&g_menubackspr);;
	g_menuscene.AddUnit(&g_menuback);

	std::string items[] = { "CONTINUE", "NEW GAME", "CREDITS", "EXIT" };
	for (int i = 0; i < 4; ++i)
		g_menu.AddItem(items[i], g_width / 2.0f - (items[i].length()*11.4f), 240 + 60.0f*i);


	g_menu.EnableItem(0, false);
	g_loadingscene.Initialize();
	g_splashscreen.LoadSprite("Splash.jpg", static_cast<float>(g_width), static_cast<float>(g_height));
	g_splash.Initialize(&g_splashscreen);
	g_audioengine = irrklang::createIrrKlangDevice();

	g_loadingscene.Resize(static_cast<float>(g_width), static_cast<float>(g_height));
	g_loadingscene.Update(0.1f);
	g_loadingscene.AddUnit(&g_splash);

	g_loadingscene.Draw();

	g_creditscene.Initialize();
	g_creditspr.LoadSprite("credits.jpg", static_cast<float>(g_width), static_cast<float>(g_height));
	g_credit.Initialize(&g_creditspr);
	g_creditscene.AddUnit(&g_credit);

	g_game.Initialize();

	g_splash.Fade(2);
}

void CleanUp()
{
	g_game.CleanUp();

	g_menubackspr.CleanUp();
	g_menuback.CleanUp();

	g_splashscreen.CleanUp();
	g_splash.CleanUp();

	g_creditspr.CleanUp();
	g_credit.CleanUp();
	g_creditscene.CleanUp();

	g_menuscene.CleanUp();
	g_loadingscene.CleanUp();
	g_renderer.CleanUp();
}

void Resize(int width, int height)
{
	g_width = width; g_height = height;
	if (g_globalState == MAINMENU)
		g_menuscene.Resize(static_cast<float>(width), static_cast<float>(height));
	if (g_globalState == CREDITS)
		g_creditscene.Resize(static_cast<float>(width), static_cast<float>(height));
	else if (g_globalState == GAME)
		g_game.Resize(width, height);
	if (g_globalState == SPLASH)
		g_loadingscene.Resize(static_cast<float>(width), static_cast<float>(height));
}

int g_menuitem;
bool g_menutransition = false;
bool g_credittransition = false;
void Update(double totalTime, double deltaTime)
{
	if (g_globalState == SPLASH)
	{
		if (!g_splash.IsFading())
		{
			g_menuback.FadeIn(2);
			g_menu.FadeIn(2);
			g_globalState = MAINMENU;
			Resize(g_width, g_height);
			g_menutransition = false;
			g_window.ResetTimer();
		}
		g_loadingscene.Update(deltaTime);
	}
	else if (g_globalState == MAINMENU)
	{
		g_audioengine->stopAllSounds();
		int mx, my;
		g_window.GetMousePos(mx, my);
		if (!g_menutransition)
		{
			int menuitem = g_menu.Check(static_cast<float>(mx), static_cast<float>(my), g_window.CheckMButton(MOUSE_LEFT));
			if (menuitem == 3) g_window.Quit();
			else if (menuitem != -1)
			{
				g_menuitem = menuitem;
				g_menutransition = true;
				g_menuback.Fade(2);
				g_menu.Fade(2);
			}
		}
		else if (!g_menuback.IsFading())
		{
			switch (g_menuitem)
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
			case 2:
				g_globalState = CREDITS;
				g_window.SetMousePos(g_width / 2, g_height / 2);
				Resize(g_width, g_height);
				g_window.ResetTimer();
				g_credit.FadeIn(2);
				break;
			case 3:
				g_window.Quit();
				break;
			}
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
			g_menutransition = false;
			g_menuback.FadeIn(2);
			g_menu.FadeIn(2);
			g_window.ShowMouseCursor(true);
			if (g_game.GetState()== Game::GAME_PLAYING) g_menu.EnableItem(0, true);
		}
	}
	else if (g_globalState == CREDITS)
	{
		if (g_window.CheckKey(VK_ESCAPE))
		{
			g_credit.Fade(2);
			g_credittransition = true;
		}
		if (g_credittransition && !g_credit.IsFading())
		{
			g_globalState = MAINMENU;
			Resize(g_width, g_height);
			g_window.ResetTimer();
			g_menutransition = false;
			g_menuback.FadeIn(2);
			g_menu.FadeIn(2);
			g_credittransition = false;
		}
		g_creditscene.Update(deltaTime);
	}
}

void Render()
{
	if (g_globalState == MAINMENU)
		g_menuscene.Draw();
	if (g_globalState == SPLASH)
		g_loadingscene.Draw();
	if (g_globalState == CREDITS)
		g_creditscene.Draw();
	else if (g_globalState == GAME)
		g_game.Render();
}
