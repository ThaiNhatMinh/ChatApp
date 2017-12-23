#pragma once

#include "header.h"
#include "SystemUI.h"
#include "Windows.h"

class Windows;
class OpenGLRenderer;
class SystemUI;
class Application
{
public:

	Application();
	~Application();
	void RunMainLoop();

protected:
	

	void RenderUI();
	std::unique_ptr<Windows> m_Window;
	std::unique_ptr<OpenGLRenderer> m_Renderer;
	std::unique_ptr<SystemUI> m_UI;
	
};

