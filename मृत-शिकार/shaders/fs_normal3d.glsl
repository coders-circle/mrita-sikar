#version 330

in vec3 fNormal;
in vec2 fTexCoord;

layout (location = 0) out vec4 fColor;

uniform sampler2D texture_sample;

void main()
{
	vec4 fTexColor = texture2D(texture_sample, fTexCoord);

	vec3 dir = normalize(vec3(-1.0, -1.0, 0.0));
    float fDiffuseIntensity = max(0, dot(normalize(fNormal), - dir ));
    fColor = vec4(vec3(0.45+fDiffuseIntensity), 1.0) * fTexColor;
}
