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
	Server* p = new Server();
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
}