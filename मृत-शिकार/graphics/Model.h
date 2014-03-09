#pragma once

/*
MODEL.H
-----------------------------
*/

#include "Mesh.h"
#include "../collision/Collision.h"

// Transition data between two animations
struct TransitionState
{
	double prevTime;
	double newTime;
	unsigned int nextSet;

	double finishTime;
};

// A structure to hold information about current animation state
// Use this to render different objects at different animation states using same model
struct ModelAnimation
{
	ModelAnimation(){ transition = NULL; set = 0; time = 0.0; }
	double time;
	unsigned int set;

	// for transition between sets
	TransitionState * transition;
};

// A model represents a collections of meshes that may or may not be animated
class Model
{
private:
	glm::mat4 m_scale;
	BoundVolume m_boundvolume;

	struct AnimatedModelInfo
	{
		Node rootnode;
		std::vector<Animation> animations;
	};

	std::vector<Mesh> m_meshes;
	glm::mat4 m_transform;

	AnimatedModelInfo * m_animationtree;

	void ReadNode(std::fstream &file, Node * node, std::map<unsigned int, Node*> &map); 
	void UpdateNode(Node * node, Node * parentnode);
	void DrawNode(Node * node);
	Renderer * m_renderer;
public:
	Model(Renderer * renderer = NULL);
	void SetRenderer(Renderer * renderer) { m_renderer = renderer; }

	void LoadModel(std::string filename);
	void Draw();
	void CleanUp();

	void SetScale(float scale) {
		m_scale = glm::scale(glm::mat4(), glm::vec3(scale));
		m_boundvolume.parent.SetCenter(m_boundvolume.parent.GetCenter() * scale);
		m_boundvolume.parent.SetExtents(m_boundvolume.parent.GetExtents() * scale);

		for (unsigned int i = 0; i < m_boundvolume.children.size(); ++i)
		{
			m_boundvolume.children[i].SetCenter(m_boundvolume.children[i].GetCenter() * scale);
			m_boundvolume.children[i].SetExtents(m_boundvolume.children[i].GetExtents() * scale);
		}
	}

	unsigned int AddMesh(Mesh &mesh);
	void SetTexture(unsigned int mesh, std::string filename);

	void SetTransform(const glm::mat4 &transform) { m_transform = transform; }
	glm::mat4 &GetTransform() { return m_transform; }

	void Transition(ModelAnimation &modelAnimation, unsigned int set, double transitionTime);
	void Advance(ModelAnimation &modelAnimation, double deltaTime, bool * animationEnded = NULL);
	void Animate(ModelAnimation &modelAnimation, bool loop = true);

	const BoundVolume& GetBoundVolume() const 
	{ return m_boundvolume; }

	void SetBoundBox(const Box &box)
	{
		m_boundvolume.parent = box;
	}
};

