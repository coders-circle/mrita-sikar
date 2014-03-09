#pragma once

/*
TECHNIQUES.H
-----------------------------
A Technique represents a set of shaders, collectively compiled as a program
*/

// A rendering technique, representing a set of shaders to use
struct Technique
{
	unsigned int program;
	unsigned int GetUniform(const char* uniformid){ return glGetUniformLocation(program, uniformid); }
};

// Technique to draw normal 3d objects to shadow depth map
struct Technique_DepthMapNormal : Technique
{
	unsigned int mvp;
	void Init()
	{
		mvp = GetUniform("mvp");
	}
};

// Technique to draw skinned 3d objects to shadow depth map
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

// Technique to draw skinned 3d objects
struct Technique_Skin3D : Technique
{
	unsigned int mvp;
	unsigned int model;
	unsigned int mbones;
	unsigned int bias_light_mvp;
	unsigned int texture_sample;
	unsigned int texture_depthmap;
	void Init() 
	{ 
		mvp = GetUniform("mvp"); 
		model = GetUniform("model");
		mbones = GetUniform("mbones");
		bias_light_mvp = GetUniform("bias_light_mvp");
		texture_sample = GetUniform("texture_sample");
		texture_depthmap = GetUniform("texture_depthmap");
	}
};

// Technique to draw normal 3d objects
struct Technique_3D : Technique
{
	unsigned int mvp;
	unsigned int model;
	unsigned int bias_light_mvp;
	unsigned int texture_sample;
	unsigned int texture_depthmap;
	void Init()
	{
		mvp = GetUniform("mvp");
		model = GetUniform("model");
		bias_light_mvp = GetUniform("bias_light_mvp");
		texture_sample = GetUniform("texture_sample");
		texture_depthmap = GetUniform("texture_depthmap");
	}
};

// Technique to draw 2d sprite/billboard
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

// Technique to draw skybox
struct Technique_Sky : Technique
{
	unsigned int mvp;
	unsigned int texture_sample;
	void Init()
	{
		mvp = GetUniform("mvp");
		texture_sample = GetUniform("texture_sample");
	}
};

// A set of techniques grouped together
struct Techniques
{
	Technique_Skin3D skin;
	Technique_3D normal3D;
	Technique_Sprite sprite;
	Technique_DepthMapNormal depthMapNormal;
	Technique_DepthMapSkin depthMapSkin;
	Technique_Sky sky;
};
