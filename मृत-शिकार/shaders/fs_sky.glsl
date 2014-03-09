#version 330
in vec3 fTexCoord;

layout (location = 0) out vec4 fColor;

uniform samplerCube texture_sample;

void main()
{
	fColor = texture(texture_sample, fTexCoord);
}