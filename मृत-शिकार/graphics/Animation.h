#pragma once

struct Node
{
	std::vector<Node> children;

	glm::mat4 transform;
	glm::mat4 combined_transform;

	std::vector<unsigned int> meshes;
};

struct VecKey
{
	double time;
	glm::vec3 vec;
};

struct RotKey
{
	double time;
	glm::quat quat;
};

struct NodeAnim
{
	Node* node;
	std::vector<VecKey> poskeys;
	std::vector<RotKey> rotkeys;
};

struct Animation
{
	double duration;
	std::vector<NodeAnim> nodeanims;

	double speed;
};

struct Bone
{
	Node* node;
	glm::mat4 offset;
};

struct WeightInfo
{
	unsigned int boneids[4];
	float weights[4];
};
