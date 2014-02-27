#pragma once
#include "Mesh.h"
#include "../collision/Collision.h"

struct TransitionState
{
	double prevTime;
	double newTime;
	unsigned int nextSet;

	double finishTime;
};

struct ModelAnimation
{
	ModelAnimation(){ transition = NULL; set = 0; time = 0.0; }
	double time;
	unsigned int set;

	// for transition between sets
	TransitionState * transition;
};

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
	void DrawNode(Node * node, unsigned int pass);
	Renderer * m_renderer;
public:
	Model(Renderer * renderer = NULL);
	void SetRenderer(Renderer * renderer) { m_renderer = renderer; }

	void LoadModel(std::string filename);
	void Draw(unsigned int pass);
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

/*	void Translate(const glm::vec3 &translation);
	void TranslateAbs(const glm::vec3 &translation);
	void Rotate(const float angle, const glm::vec3 &axis);
	void RotateAbs(const float angle, const glm::vec3 &axis);
	void ResetTransformation();*/
};

