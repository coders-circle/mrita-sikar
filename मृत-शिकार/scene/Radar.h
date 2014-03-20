#pragma once
#include "GameScene.h"
#include "Unit2d.h"
class Radar : public Unit2d
{
private:
	Sprite m_sprZombie;// , m_spr2, m_spr3;
	const GameScene * m_scene;
	const Unit * m_player;
	float m_radius;
public:
	void Initialize(Sprite * sprite, glm::vec2 position = glm::vec2(), bool animate = false);
	void CleanUp() {
		m_sprZombie.CleanUp();
	}
	
	void SetRadarRadius(float radius) { m_radius = radius; }
	void SetScene(const GameScene * scene) {
		m_scene = scene;
		m_sprZombie.SetRenderer(m_scene->GetRenderer());
	}
	void SetPlayer(const Unit * player) { m_player = player; }
	void Update(double deltaTime) {}
	void Draw();
};

