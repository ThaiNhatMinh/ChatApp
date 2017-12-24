#include "stdafx.h"
#include "Application.h"


Application::Application()
{
	
	m_Window = std::unique_ptr<Windows>(new Windows());
	m_Renderer = std::unique_ptr<OpenGLRenderer>(new OpenGLRenderer(m_Window.get()));
	m_UI = std::unique_ptr<SystemUI>(new SystemUI(m_Window.get()));


	

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);


}


Application::~Application()
{
	
}

void Application::RunMainLoop()
{
	

	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (!m_Window->ShouldClose())
	{
		glfwPollEvents();
		
		m_Renderer->Clear();

		RenderUI();
		
		if (m_pModule) m_pModule->Update(0);

		ImGui::Render();
	
		m_Renderer->SwapBuffer();
	}
}

void Application::RenderUI()
{
	m_UI->NewFrame();
}
