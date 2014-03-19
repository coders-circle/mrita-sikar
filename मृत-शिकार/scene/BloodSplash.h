#pragma once
#include "Unit2d.h"
class BloodSplash : public Unit2d
{
private:
	struct Splash
	{
		glm::vec2 position;
		double time;
		unsigned int imageId;
		double fadetime;
	};
	std::vector<Splash> m_splashes;
public:
	void CleanUp() { m_splashes.clear(); }

	void Update(double deltaTime) 
	{
		for (unsigned int i = 0; i < m_splashes.size(); ++i)
		{
			m_splashes[i].time += deltaTime;
			if (m_splashes[i].time >= m_splashes[i].fadetime)
			{
				m_splashes.erase(m_splashes.begin() + i);
				--i;
			}
		}
	}

	void Draw()
	{
		for (unsigned int i = 0; i < m_splashes.size(); ++i)
			m_sprite->DrawSprite(m_splashes[i].imageId, m_splashes[i].position.x, m_splashes[i].position.y, 1.0f, 1.0f - static_cast<float>(m_splashes[i].time / m_splashes[i].fadetime));
	}

	void AddSplash(double fadetime)
	{
		Splash splash;
		splash.imageId = rand() % 9;
		splash.position.x = rand() % 5 * 90.0f + 400.0f;//(rand() % ((windowWidth - 400) / 80)) * 80.0f + windowWidth / 2.0f - 600.0f;
		splash.position.y = rand() % 6 * 90.0f + 240.0f;//(rand() % ((windowHeight - 400) / 80)) * 80.0f + windowHeight / 2.0f - 200.0f;
		splash.time = 0.0;
		splash.fadetime = fadetime;
		m_splashes.push_back(splash);
	}

};
