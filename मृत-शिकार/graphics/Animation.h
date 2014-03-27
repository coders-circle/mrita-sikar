#pragma once

/*
ANIMATION.H
-----------------------------
Contains structures to hold information about model animations
*/

// A node in the animation tree
struct Node
{
	std::vector<Node> children;

	glm::mat4 transform;
	glm::mat4 combined_transform;

	std::vector<unsigned int> meshes;
};

// Animation key-frame to store position (and maybe scaling) information
struct VecKey
{
	double time;
	glm::vec3 vec;
};

// Animation key-frame to store rotation information
struct RotKey
{
	double time;
	glm::quat quat;
};


// Keyframes for a node
struct NodeAnim
{
	Node* node;
	std::vector<VecKey> poskeys;
	std::vector<RotKey> rotkeys;
};

// A single animation
struct Animation
{
	double duration;
	std::vector<NodeAnim> nodeanims;

	double speed;
};

// A bone that may be attached to vertices and holds reference to a node in the animation tree
struct Bone
{
	Node* node;
	glm::mat4 offset;
};

// Bone attachment information for each vertex, consisting four bones to attach to and weights for each bone
struct WeightInfo
{
	unsigned int boneids[4];
	float weights[4];
};
