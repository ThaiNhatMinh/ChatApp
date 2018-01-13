#include "stdafx.h"
#include "ServerApp.h"

WNDPROC g_MainWndProc;
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR lpUserData = GetWindowLongPtr(hWnd, GWLP_USERDATA);
	Server* pMyObject = (Server*)lpUserData;
	
	if(message== WM_SOCKET)
	{
		pMyObject->ProcessEvent(wParam, lParam);
	}

	return CallWindowProc(g_MainWndProc, hWnd, message, wParam, lParam);
}

ServerApp::ServerApp()
{
	Socket::InitWinShock();
	Server* p = new Server(m_Logger);
	if (!p->Init())
	{
		printf("Cannot init server.");
		exit(0);
	}


	m_pModule = std::unique_ptr<Server>(p);

	HWND hWnd = m_Window->GetHandle();
	p->SetAsynch(hWnd);
	g_MainWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)MainWndProc);
	SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG)p);
	
}


ServerApp::~ServerApp()
{
	printf("DDDD");
}

void ServerApp::RunMainLoop()
{
	m_Window->ShowWindows();
	while (!m_Window->ShouldClose())
	{
		glfwPollEvents();

		m_Renderer->Clear();

		RenderUI();
		//Sleep(1000);
		if (m_pModule) m_pModule->Update(0);

		ImGui::Render();

		m_Renderer->SwapBuffer();
	}
}

void ServerApp::RenderUI()
{
	m_UI->NewFrame();

	m_Logger.Draw("Server Log");
}
