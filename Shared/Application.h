#pragma once

#include "header.h"
#include "SystemUI.h"
#include "Windows.h"

class Windows;
class OpenGLRenderer;
class SystemUI;

class Module
{
public:
	virtual ~Module() {};
	virtual void Update(float dt) = 0;

};
class Application
{
public:

	Application();
	~Application();
	virtual void RunMainLoop();
protected:
	virtual void RenderUI();
protected:
	
	std::unique_ptr<Module> m_pModule;
	
	std::unique_ptr<Windows> m_Window;
	std::unique_ptr<OpenGLRenderer> m_Renderer;
	std::unique_ptr<SystemUI> m_UI;
	
};

