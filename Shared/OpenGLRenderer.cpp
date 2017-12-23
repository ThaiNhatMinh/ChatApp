#include "stdafx.h"
#include "OpenGLRenderer.h"

OpenGLRenderer::OpenGLRenderer(Windows* w):m_RenderName("OpenGL"),m_ClearColor(0.1f,0.1f,0.1f,1.0f),m_iClearFlag(0),m_DrawMode(GL_TRIANGLES)
{
	

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::string Error = std::string("glewInit failed! ");
		std::cout<<Error;
		return;
	}

	vec2 size = w->GetWindowSize();
	m_glfwWindow = w->Window();


	m_Viewport = vec4(0, 0, size.x, size.y);
	// Define the viewport dimensions
	glViewport(m_Viewport.x, m_Viewport.y, m_Viewport.z, m_Viewport.w);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	//glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	SetClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

OpenGLRenderer::~OpenGLRenderer()
{
}

bool OpenGLRenderer::ReadConfig(tinyxml2::XMLElement *pRenderer)
{
	/*
	
	*/

	tinyxml2::XMLElement* pClearColor = pRenderer->FirstChildElement("ClearColor");

	m_ClearColor.x = pClearColor->DoubleAttribute("r", 0.3);
	m_ClearColor.y = pClearColor->DoubleAttribute("g", 0.3);
	m_ClearColor.z = pClearColor->DoubleAttribute("b", 0.3);
	m_ClearColor.w = pClearColor->DoubleAttribute("a", 1.0);

	

	return true;
}


void OpenGLRenderer::ClearBuffer()
{
	glClear(m_iClearFlag);
}

void OpenGLRenderer::ClearColor()
{
	glClearColor(m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w);
}

void OpenGLRenderer::Clear()
{
	ClearBuffer();
	ClearColor();
}

void OpenGLRenderer::Draw(GLint first, GLsizei count, GLsizei primcount)
{
	if (primcount) glDrawArraysInstanced(m_DrawMode, first, count, primcount);
	else glDrawArrays(m_DrawMode, first, count);
}

void OpenGLRenderer::DrawElement(GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
	if (primcount) glDrawElementsInstanced(m_DrawMode, count, type, indices, primcount);
	else glDrawElements(m_DrawMode, count, type, indices);
}

void OpenGLRenderer::SetClearFlag(unsigned int flag)
{
	m_iClearFlag = flag;
}

void OpenGLRenderer::SetTexture(Texture * p, GLuint tex_unit)
{
	assert(p != nullptr);

	glActiveTexture(GL_TEXTURE0 + tex_unit);
	glBindTexture(GL_TEXTURE_2D, p->iIndex);

}

void OpenGLRenderer::SetViewport(int x, int y, int weight, int height)
{
	m_Viewport = vec4(x, y, weight, height);
	glViewport(x, y, weight, height);
}

void OpenGLRenderer::SetViewport(vec4 v)
{
	m_Viewport = v;
	glViewport(v.x, v.y, v.z, v.w);
}

void OpenGLRenderer::SetDrawMode(GLenum mode)
{
	m_DrawMode = mode;
}

void OpenGLRenderer::SetVertexArrayBuffer(GLuint vao)
{
	glBindVertexArray(vao);
}

vec4 OpenGLRenderer::GetViewport()
{
	return m_Viewport;
}

void OpenGLRenderer::SwapBuffer()
{
	glfwSwapBuffers(m_glfwWindow);
	
}
