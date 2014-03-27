#version 330

layout (location = 0) in vec4 vPosition;
layout (location = 3) in ivec4 boneids;
layout (location = 4) in vec4 weights;

uniform mat4 mvp;
uniform mat4 mbones[100];

void main()
{
	mat4 bonetransform = mbones[boneids[0]] * weights[0];
	bonetransform += mbones[boneids[1]] * weights[1];
	bonetransform += mbones[boneids[2]] * weights[2];
	bonetransform += mbones[boneids[3]] * weights[3];

    gl_Position = mvp * bonetransform * vPosition;
}