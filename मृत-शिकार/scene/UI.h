#pragma once

#include "Scene.h"


class MenuItem
{
private:
	int m_normalTextIndex;
	int m_hoveredTextIndex;
	Scene* m_scene;
	float m_x1, m_y1, m_x2, m_y2;
public:
	MenuItem():m_normalTextIndex (-1), m_hoveredTextIndex(-1){ m_scene = 0; }
	void Set(Scene* s, float x, float y, std::string text)
	{
		m_scene = s;
		m_normalTextIndex = s->AddText(Text(text, x, y, 1.0f, false));
		float ss = 16.0f*text.length();
		m_hoveredTextIndex = s->AddText(Text(text, x - ss*0.2f, y - 16.0f*0.2f, 1.2f, false));
		m_x1 = x; m_y1 = y;
		m_x2 = m_x1 + ss*2.0f; m_y2 = m_y1 + 32.0f;
	}
	void OnMouseHover()
	{
		m_scene->SetTextVisible(m_hoveredTextIndex, true);
		m_scene->SetTextVisible(m_normalTextIndex, false);
	}
	void OnMouseLeave()
	{
		m_scene->SetTextVisible(m_hoveredTextIndex, false);
		m_scene->SetTextVisible(m_normalTextIndex, true);
	}
	bool Check(float mx, float my)
	{
		if (mx > m_x1 && mx < m_x2 && my > m_y1 && my < m_y2){ OnMouseHover(); return true; }
		OnMouseLeave();
		return false;
	}
	void Hide()
	{
		m_scene->SetTextVisible(m_hoveredTextIndex, false);
		m_scene->SetTextVisible(m_normalTextIndex, false);
	}
	void Show()
	{
		m_scene->SetTextVisible(m_normalTextIndex, true);
	}
};

enum MenuOrientation{Horizontal, Vertical};

class Menu
{
private:
	std::vector<MenuItem> m_items;
	Scene* m_scene;
	Sprite m_background;
	float m_sx, m_sy;
	float m_offset;
	int m_orientation;
public:
	Menu() :m_orientation(Vertical), m_offset(50.0f)
	{}
	void SetScene(Scene* scene){ m_scene = scene; }
	void Initialize(Scene* scene, std::string backgroundImage)
	{
		SetScene(scene);
		m_background.LoadSprite(backgroundImage, 1366.0f, 768.0f);
	}
	void AddItem(std::string menuText, float x, float y)
	{
		m_items.resize(m_items.size() + 1);
		m_items[m_items.size() - 1].Set(m_scene, x, y, menuText);
	}
	int Check(float mx, float my, bool mlbuttondown)
	{
		for (int i = 0; i < m_items.size(); i++)
		{
			if (m_items[i].Check(mx, my) && mlbuttondown)
				return i;
		}
		return -1;
	}
};

class UI
{
private:
	Menu m_mainMenu;
	Scene* scene;
public:
	UI()
	{
	}
	void Initialize(Scene* scene)
	{

	}
};