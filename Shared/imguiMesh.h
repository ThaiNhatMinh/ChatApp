#pragma once
#include "header.h"

struct imguiVertex
{
	vec2 pos;
	vec2 uv;
	vec4 color;
};

class imguiMesh 
{
public:
	GLuint VAO, VBO, EBO;
	GLenum	Topology;
	std::vector<imguiVertex>	m_Vertexs;
	std::vector<unsigned int>	m_Indices;
public:
	imguiMesh();
	~imguiMesh();

	// dirive class can redifine this function to add more data;
	virtual void Finalize(Shader* p);
};

struct DefaultVertex
{
	vec2 pos;
	vec2 uv;
};

class Mesh 
{
public:
	GLuint VAO, VBO;
	GLenum	Topology;
	std::vector<DefaultVertex>	m_Vertexs;
public:
	Mesh();
	virtual ~Mesh();

	// dirive class can redifine this function to add more data;
	void Finalize(Shader* p);
	virtual void Resize(float w, float h)=0;
	
};

class ImageMesh:public Mesh
{
public:
	ImageMesh() {};
	~ImageMesh() {};
	virtual void Resize(float w, float h);
};

class ScreenMesh :public Mesh
{
public:
	GLint W, H;
	ScreenMesh() {};
	~ScreenMesh() {};
	virtual void Resize(float w, float h);
	void UpdateBuffer();
};