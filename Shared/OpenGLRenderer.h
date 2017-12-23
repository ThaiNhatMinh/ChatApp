#pragma once

#include "header.h"

class OpenGLRenderer 
{
	friend class Scene;
private:
	std::string			m_RenderName;
	glm::vec4			m_ClearColor;
	glm::vec4			m_Viewport;
	bool			m_HasInit;
	unsigned int	m_iClearFlag;
	GLenum			m_DrawMode;
	GLFWwindow*		m_glfwWindow;
public:
	OpenGLRenderer(Windows* w);
	~OpenGLRenderer();

	virtual void ClearBuffer();
	virtual void ClearColor();
	virtual void Clear();

	virtual void Draw(GLint first, GLsizei count, GLsizei primcount = 0);
	virtual void DrawElement(GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount = 0);

	virtual void SetClearFlag(unsigned int flag);
	virtual void SetTexture(Texture* p, GLuint tex_unit = 0);
	virtual void SetViewport(int x, int y, int weight, int height);
	virtual void SetViewport(vec4 v);
	virtual void SetDrawMode(GLenum mode);
	virtual void SetVertexArrayBuffer(GLuint vao);
	virtual vec4 GetViewport();
	virtual vec4& GetClearColor() {
		return m_ClearColor;
	};
	virtual void SwapBuffer();

private:
	

	bool ReadConfig(tinyxml2::XMLElement *pData);
};