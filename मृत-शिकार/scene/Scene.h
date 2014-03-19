#pragma once
#include "Unit2d.h"
#include "../graphics/graphics.h"

struct Text
{
	Text(std::string text, float posX, float posY, float size=1.0f) { this->text = text; x = posX; y = posY; this->size = size; }
	std::string text;
	float x, y, size;
};

class Scene
{
protected:
	Renderer * m_renderer;
	std::vector<Unit2d*> m_unit2ds;
	Sprite m_font;
	std::vector<Text> m_texts;

	void DrawTexts()
	{
		for (unsigned i = 0; i < m_texts.size(); ++i)
		{
			float py = m_texts[i].y, px = 0.0f;;
			for (unsigned j = 0; j < m_texts[i].text.size(); ++j)
			{
				if (m_texts[i].text[j] == '\n')
				{
					py += 32.0f * m_texts[i].size; px = 0.0f;
				}
				else
				{
					m_font.DrawSprite(m_texts[i].text[j], m_texts[i].x + px, py, m_texts[i].size);
					px += (32.0f - 7.0f) * m_texts[i].size;
				}
			}
		}
	}

public:
	Scene(Renderer * renderer = NULL);
	virtual void SetRenderer(Renderer * renderer) { m_renderer = renderer; m_font.SetRenderer(renderer); }
	Renderer* GetRenderer() { return m_renderer; }

	void AddUnit(Unit2d * unit2d) { m_unit2ds.push_back(unit2d); }
	const std::vector<Unit2d*> &GetUnit2ds() const { return m_unit2ds; }

	unsigned int AddText(const Text &text) { m_texts.push_back(text); return m_texts.size() - 1; }
	void ChangeText(unsigned int index, std::string text) { m_texts[index].text = text; }
	void RemoveText(unsigned int index) { m_texts.erase(m_texts.begin() + index); }

	void Initialize()
	{
		m_font.LoadSprite("font1.png", 32, 32, 0, 0, 16, 16);
	}

	virtual void Resize(float width, float height) {}
	virtual void Update(double deltaTime) {
		for (unsigned i = 0; i < m_unit2ds.size(); ++i)
			m_unit2ds[i]->Update(deltaTime);
	}

	virtual void Draw() {
		m_renderer->BeginRender(Renderer::NORMAL_PASS);

		m_renderer->DisableDepth();
		for (unsigned i = 0; i < m_unit2ds.size(); ++i)
			m_unit2ds[i]->Draw();
		DrawTexts();

		m_renderer->EndRender();
	}
	virtual void CleanUp() {
		m_font.CleanUp();
		m_texts.clear();
		m_unit2ds.clear();
	}
};