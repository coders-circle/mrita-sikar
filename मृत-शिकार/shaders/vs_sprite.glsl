#version 330

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoord;

uniform mat4 mvp;
uniform vec2 uv;

out vec2 fTexCoord;

void main()
{
	gl_Position = mvp * vPosition;
	fTexCoord = vTexCoord + uv;
}
