#pragma once

struct Technique
{
	unsigned int program;
	unsigned int GetUniform(const char* uniformid){ return glGetUniformLocation(program, uniformid); }
};

struct Technique_DepthMapNormal : Technique
{
	unsigned int mvp;
	void Init()
	{
		mvp = GetUniform("mvp");
	}
};

struct Technique_DepthMapSkin : Technique
{
	unsigned int mvp;
	unsigned int mbones;
	void Init()
	{
		mvp = GetUniform("mvp");
		mbones = GetUniform("mbones");
	}
};

struct Technique_Skin3D : Technique
{
	unsigned int mvp;
	unsigned int model;
	unsigned int mbones;
	unsigned int bias_light_mvp;
	unsigned int texture_sample;
	void Init() 
	{ 
		mvp = GetUniform("mvp"); 
		model = GetUniform("model");
		mbones = GetUniform("mbones");
		bias_light_mvp = GetUniform("bias_light_mvp");
		texture_sample = GetUniform("texture_sample");
	}
};

struct Technique_3D : Technique
{
	unsigned int mvp;
	unsigned int model;
	unsigned int bias_light_mvp;
	unsigned int texture_sample;
	void Init()
	{
		mvp = GetUniform("mvp");
		model = GetUniform("model");
		bias_light_mvp = GetUniform("bias_light_mvp");
		texture_sample = GetUniform("texture_sample");
	}
};

struct Technique_Sprite : Technique
{
	unsigned int mvp;
	unsigned int uv;
	unsigned int texture_sample;
	void Init()
	{
		mvp = GetUniform("mvp");
		uv = GetUniform("uv");
		texture_sample = GetUniform("texture_sample");
	}
};

struct Techniques
{
	Technique_Skin3D skin;
	Technique_3D normal3D;
	Technique_Sprite sprite;
	Technique_DepthMapNormal depthMapNormal;
	Technique_DepthMapSkin depthMapSkin;
};
