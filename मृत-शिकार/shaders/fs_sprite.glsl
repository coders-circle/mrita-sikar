#version 330
in vec2 fTexCoord;

layout (location = 0) out vec4 fColor;

uniform sampler2D texture_sample;

void main()
{
	fColor = texture2D(texture_sample, fTexCoord);
}