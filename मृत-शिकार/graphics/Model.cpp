#include "stdinc.h"
#include "TextureLoader.h"
#include "Model.h"

Model::Model(Renderer * renderer) : m_animationtree(NULL), m_renderer(renderer)
{
}


void Model::CleanUp()
{
	if (m_animationtree)
		delete m_animationtree;

	for (unsigned i = 0; i < m_meshes.size(); ++i)
	{
		unsigned int t = m_meshes[i].GetTexture();
		glDeleteTextures(1, &t);
		m_meshes[i].CleanUp();
	}
}

#define READ_SZ_FILE(x) file.read((char*)&data, sizeof(data)); x.resize(data)
#define READ_SZ_FILE_(x) file.read((char*)&data, sizeof(data)); x->resize(data)
#define READ_FILE(x) file.read((char*)&x, sizeof(x))
#define READ_FILE_EX(x, y) file.read((char*)x, y);

void Model::ReadNode(std::fstream &file, Node * node, std::map<unsigned int, Node*> &map)
{
	unsigned int data;
	READ_FILE(data);
	map[data] = node;
	READ_SZ_FILE(node->meshes);
	if (data>0) READ_FILE_EX(&node->meshes[0], node->meshes.size() * sizeof(unsigned int));
	READ_SZ_FILE(node->children);
	for (unsigned i = 0; i < node->children.size(); i++)
		ReadNode(file, &node->children[i], map);
}

void Model::LoadModel(std::string filename)
{
	std::fstream file;

	file.open("models\\" + filename, std::fstream::in | std::fstream::binary);
	if (file.fail())
	{
		std::cout << "Couldn't open file for loading: " << filename << std::endl;
		return;
	}

	std::map<unsigned int, Node*> map;

	bool tree;
	READ_FILE(tree);
	if (tree)
	{
		m_animationtree = new AnimatedModelInfo;
		ReadNode(file, &m_animationtree->rootnode, map);
	}
	unsigned int data;

	std::vector<SkinVertex> svertices;
	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;

	READ_SZ_FILE(m_meshes);
	for (unsigned i = 0; i < m_meshes.size(); ++i)
	{
		unsigned int bns_sz;
		READ_FILE(bns_sz);

		if (bns_sz > 0 && tree)
		{
			READ_SZ_FILE(svertices);
			if (data > 0) READ_FILE_EX(&svertices[0], svertices.size() * sizeof(SkinVertex));
		}
		else
		{
			READ_SZ_FILE(vertices);
			if (data > 0) READ_FILE_EX(&vertices[0], vertices.size() * sizeof(Vertex));
		}

		READ_SZ_FILE(indices);
		if (data > 0) READ_FILE_EX(&indices[0], indices.size() * sizeof(unsigned short));

		if (bns_sz > 0 && tree)
		{
			m_meshes[i].LoadData(svertices, indices);
			std::vector<Bone> * &bones = m_meshes[i].GetBones();
			bones->resize(bns_sz);
			for (unsigned j = 0; j < bones->size(); ++j)
			{
				Bone * bn = &(*bones)[j];
				READ_FILE(data);
				bn->node = map[data];
				READ_FILE_EX(&bn->offset[0][0], 4 * 4 * sizeof(float));
			}
		}
		else 
			m_meshes[i].LoadData(vertices, indices);

		char str[100];
		READ_FILE(data);
		READ_FILE_EX(str, data); 
		str[data] = '\0';
		m_meshes[i].SetTexture(LoadTexture("textures\\" +std::string(str)));
		m_meshes[i].SetRenderer(m_renderer);
	}

	if (tree)
	{
		READ_SZ_FILE(m_animationtree->animations);
		for (unsigned i = 0; i < m_animationtree->animations.size(); ++i)
		{
			Animation * anim = &m_animationtree->animations[i];
			READ_FILE(anim->duration);
			READ_FILE(anim->speed);
			READ_SZ_FILE(anim->nodeanims);
			for (unsigned j = 0; j < anim->nodeanims.size(); ++j)
			{
				NodeAnim * nodeanim = &anim->nodeanims[j];
				READ_FILE(data);
				nodeanim->node = map[data];
				READ_SZ_FILE(nodeanim->poskeys);
				if (data > 0) READ_FILE_EX(&nodeanim->poskeys[0], nodeanim->poskeys.size() * sizeof(VecKey));
				READ_SZ_FILE(nodeanim->rotkeys);
				if (data > 0) READ_FILE_EX(&nodeanim->rotkeys[0], nodeanim->rotkeys.size() * sizeof(RotKey));
			}
		}
	}

	READ_FILE(m_boundvolume.parent); 
	READ_SZ_FILE(m_boundvolume.children);
	if (data > 0) READ_FILE_EX(&m_boundvolume.children[0], m_boundvolume.children.size() * sizeof(Box));

	file.close();
}

unsigned int Model::AddMesh(Mesh &mesh)
{
	m_meshes.push_back(mesh);
	return m_meshes.size() - 1;
}

void Model::SetTexture(unsigned int mesh, std::string filename)
{
	m_meshes[mesh].SetTexture(LoadTexture("textures\\" + filename));
}

void Model::UpdateNode(Node * node, Node * parentnode)
{
	if (parentnode) node->combined_transform = parentnode->combined_transform * node->transform;
	else node->combined_transform = node->transform;
	unsigned int i;
	for (i = 0; i<node->children.size(); i++)
		UpdateNode(&node->children[i], node);
}

void Model::DrawNode(Node * node, unsigned int pass)
{
	unsigned int i;
	for (i = 0; i<node->meshes.size(); ++i)
		m_meshes[node->meshes[i]].Draw(m_transform * m_scale * node->combined_transform, pass);
	for (i = 0; i<node->children.size(); ++i)
		DrawNode(&node->children[i], pass);
}

void Model::Draw(unsigned int pass)
{
	if (m_animationtree) 
		DrawNode(&m_animationtree->rootnode, pass);
	else
	for (unsigned i = 0; i < m_meshes.size(); ++i)
		m_meshes[i].Draw(m_transform * m_scale, pass);
}

void Model::Transition(ModelAnimation &modelAnimation, unsigned int set, double transitionTime)
{
	// Do this to prevent negative value for (animTime - modelAnimation.transition->prevTime) in Animate function
	while (modelAnimation.time >= m_animationtree->animations[modelAnimation.set].duration)
		modelAnimation.time -= m_animationtree->animations[modelAnimation.set].duration;

	if (modelAnimation.transition) delete modelAnimation.transition;

	if (transitionTime == 0){ modelAnimation.set = set; modelAnimation.time = 0.0; modelAnimation.transition = NULL; return; }

	modelAnimation.transition = new TransitionState;
	modelAnimation.transition->prevTime = modelAnimation.time;
	modelAnimation.transition->nextSet = set;
	modelAnimation.transition->newTime = 0.0;

	double nowspeed = m_animationtree->animations[modelAnimation.set].speed;
	double nextspeed = m_animationtree->animations[modelAnimation.set].speed;
	modelAnimation.transition->finishTime = transitionTime;
}

void Model::Advance(ModelAnimation &modelAnimation, double deltaTime, bool * animationEnded)
{
	if (modelAnimation.transition)
	{
		double nowspeed = m_animationtree->animations[modelAnimation.set].speed;
		double nextspeed = m_animationtree->animations[modelAnimation.set].speed;
		modelAnimation.time += deltaTime * (nowspeed + (nextspeed - nowspeed)*(modelAnimation.transition->newTime / modelAnimation.transition->finishTime));
	}
	else
		modelAnimation.time += deltaTime * m_animationtree->animations[modelAnimation.set].speed;

	if (animationEnded) if (modelAnimation.time >= m_animationtree->animations[modelAnimation.set].duration) *animationEnded = true;
}

void Model::Animate(ModelAnimation &modelAnimation, bool loop)
{
	if (modelAnimation.set >= m_animationtree->animations.size()) return;

	Animation * anim = &m_animationtree->animations[modelAnimation.set];
	double &animTime = modelAnimation.time;

	if (modelAnimation.transition)
	{
		modelAnimation.transition->newTime += animTime - modelAnimation.transition->prevTime;
		animTime = modelAnimation.transition->prevTime;
	}

	while (animTime >= anim->duration)	if (loop)  animTime -= anim->duration; else animTime = anim->duration - 0.00001;

	unsigned int i, j;
	for (i = 0; i < anim->nodeanims.size(); ++i)
	{
		NodeAnim * nodeanim = &anim->nodeanims[i];
		if (!nodeanim->node) continue;
		unsigned int pk = 0, rk = 0;

		if (nodeanim->poskeys.size() > 1)
		for (j = 0; j < nodeanim->poskeys.size() - 1; ++j)
		if (nodeanim->poskeys[j + 1].time > animTime)
		{
			pk = j;
			break;
		}

		if (nodeanim->rotkeys.size() > 1)
		for (j = 0; j < nodeanim->rotkeys.size() - 1; ++j)
		if (nodeanim->rotkeys[j + 1].time > animTime)
		{
			rk = j;
			break;
		}

		unsigned int npk = (pk == nodeanim->poskeys.size() - 1) ? 0 : pk + 1;
		unsigned int nrk = (rk == nodeanim->rotkeys.size() - 1) ? 0 : rk + 1;

		double dft = (nodeanim->poskeys[npk].time - nodeanim->poskeys[pk].time);
		double drt = (nodeanim->rotkeys[nrk].time - nodeanim->rotkeys[rk].time);
		float pf = 0.0f, rf = 0.0f;

		if (dft != 0) pf = (float)((animTime - nodeanim->poskeys[pk].time) / dft);
		if (drt != 0) rf = (float)((animTime - nodeanim->rotkeys[rk].time) / drt);


		if (pf<0) pf *= -1;
		if (rf<0) rf *= -1;


		glm::vec3 pos = nodeanim->poskeys[pk].vec + pf*(nodeanim->poskeys[npk].vec - nodeanim->poskeys[pk].vec);
		glm::quat rot = glm::slerp(nodeanim->rotkeys[rk].quat, nodeanim->rotkeys[nrk].quat, rf);

		if (modelAnimation.transition)
		{
			TransitionState * ts = modelAnimation.transition;
			float factor = (float)(ts->newTime / ts->finishTime);
			if (factor != 0.0f)
			{
				if (factor > 1) factor = 1;
				unsigned int k;
				NodeAnim * newnodeanim = NULL;
				for (k = 0; k < m_animationtree->animations[ts->nextSet].nodeanims.size(); ++k)
				{
					newnodeanim = &m_animationtree->animations[ts->nextSet].nodeanims[k];
					if (newnodeanim->node == nodeanim->node) break;
				}
				if (k < m_animationtree->animations[ts->nextSet].nodeanims.size())
				{
					pos = pos + factor * (newnodeanim->poskeys[0].vec - pos);
					rot = glm::slerp(rot, newnodeanim->rotkeys[0].quat, factor);
				}
				if (factor >= 1)
				{
					modelAnimation.set = ts->nextSet;
					modelAnimation.time = 0.0;
					delete modelAnimation.transition;
					modelAnimation.transition = NULL;
				}
			}
		}

		nodeanim->node->transform = glm::translate(glm::mat4(), pos) * glm::mat4_cast(rot);

	}

	UpdateNode(&m_animationtree->rootnode, NULL);
}
/*
void Model::Translate(const glm::vec3 &translation)
{
	m_transform = glm::translate(m_transform, translation);
}

void Model::TranslateAbs(const glm::vec3 &translation)
{
	m_transform = glm::translate(glm::mat4(), translation);
}

void Model::Rotate(const float angle, const glm::vec3 &axis)
{
	m_transform = glm::rotate(m_transform, angle, axis);
}

void Model::RotateAbs(const float angle, const glm::vec3 &axis)
{
	m_transform = glm::rotate(glm::mat4(), angle, axis);
}

void Model::ResetTransformation()
{
	m_transform = glm::mat4(1.0f);
}
*/