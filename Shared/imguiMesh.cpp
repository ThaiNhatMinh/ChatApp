#include "stdafx.h"
#include"imguiMesh.h"

imguiMesh::imguiMesh()
{
}

imguiMesh::~imguiMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void imguiMesh::Finalize(Shader * p)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

	GLuint location = p->GetAttribLocation("Position");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
	location = p->GetAttribLocation("UV");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
	location = p->GetAttribLocation("Color");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

	Topology = GL_TRIANGLES;
}

Mesh::Mesh()
{
	m_Vertexs.resize(4);
	
	m_Vertexs[0].pos = vec2(-1.0f, 1.0f);
	m_Vertexs[1].pos = vec2(-1.0f, -1.0f);
	m_Vertexs[2].pos = vec2(1.0f, 1.0f);
	m_Vertexs[3].pos = vec2(1.0f, -1.0f);

	
	m_Vertexs[0].uv = vec2(0.0f, 1.0f);
	m_Vertexs[1].uv = vec2(0.0f, 0.0f);
	m_Vertexs[2].uv = vec2(1.0f, 1.0f);
	m_Vertexs[3].uv = vec2(1.0f, 0.0f);
}


Mesh::~Mesh()
{
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	
}

void Mesh::Finalize(Shader* p)
{

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_Vertexs.size() * sizeof(DefaultVertex), &m_Vertexs[0], GL_STREAM_DRAW);

	
	size_t stride = sizeof(DefaultVertex);


	size_t offset = 0;

	GLuint location = p->GetAttribLocation("Position");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
	offset += 2 * sizeof(float);



	location = p->GetAttribLocation("UV");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
	offset += 2 * sizeof(float);
	location++;


	glBindVertexArray(0);
	
	Topology = GL_TRIANGLE_STRIP;
}

void ScreenMesh::Resize(float w, float h)
{
	m_Vertexs[0].pos = vec2(-0.5f*w, 0.5f*h);
	m_Vertexs[1].pos = vec2(-0.5f*w, -0.5f*h);
	m_Vertexs[2].pos = vec2(0.5f*w, 0.5f*h);
	m_Vertexs[3].pos = vec2(0.5f*w, -0.5f*h);

	this->W = w;
	this->H = h;
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,4*sizeof(DefaultVertex), &m_Vertexs[0],GL_STREAM_DRAW);
	glBindVertexArray(0);
}

void ScreenMesh::UpdateBuffer()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(DefaultVertex), &m_Vertexs[0], GL_STREAM_DRAW);
	glBindVertexArray(0);
}

void ImageMesh::Resize(float w, float h)
{
	m_Vertexs[0].pos = vec2(0,0);
	m_Vertexs[1].pos = vec2(0, h);
	m_Vertexs[2].pos = vec2(w, 0);
	m_Vertexs[3].pos = vec2(w, h);

	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(DefaultVertex), &m_Vertexs[0], GL_STREAM_DRAW);
}