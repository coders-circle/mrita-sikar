#version 330

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 bias_light_mvp;

out vec3 fNormal;
out vec2 fTexCoord;
out vec4 fShadowCoord;

void main()
{
	gl_Position = mvp * vPosition;
	fNormal = mat3(model) * vNormal;				//transpose(inverse(mat3(model))) * vNormal;
	fTexCoord = vTexCoord;

	fShadowCoord = bias_light_mvp * vPosition;
}