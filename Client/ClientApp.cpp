#include "stdafx.h"
#include "ClientApp.h"


ClientApp::ClientApp():ShowLogin(1),ShowRegister(0),ShowChat(0)
{
	p = new Client();
	if (!p->Init())
	{
		printf("Cannot init client.");
		exit(0);
	}


	m_pModule = std::unique_ptr<Client>(p);

}


ClientApp::~ClientApp()
{
}

void ClientApp::RenderUI()
{
	m_UI->NewFrame();

	
	
	if (ShowLogin)
	{
		char InputBuf[MAX_USERNAME_LEN];
		char InputBuf2[MAX_USERNAME_LEN];

		ZeroMemory(InputBuf, MAX_USERNAME_LEN);
		ZeroMemory(InputBuf2, MAX_USERNAME_LEN);
		ImGui::SetNextWindowSize(ImVec2(250, 200), ImGuiCond_FirstUseEver);
		ImGui::Begin("Login", &ShowLogin, ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoResize| ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoCollapse);
		ImGui::InputText("Username", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::InputText("Password", InputBuf2, IM_ARRAYSIZE(InputBuf2), ImGuiInputTextFlags_EnterReturnsTrue| ImGuiInputTextFlags_Password);
		if (ImGui::Button("Login"))
		{
			p->Login(InputBuf, InputBuf2);
		}
		if (ImGui::Button("Register"))
		{
			ShowRegister = !ShowRegister;
		}
		ImGui::End();
	}
	if (ShowRegister)
	{
		char InputBuf[MAX_USERNAME_LEN];
		char InputBuf2[MAX_USERNAME_LEN];
		ZeroMemory(InputBuf, MAX_USERNAME_LEN);
		ZeroMemory(InputBuf2, MAX_USERNAME_LEN);
		ImGui::SetNextWindowSize(ImVec2(250, 200), ImGuiCond_FirstUseEver);
		ImGui::Begin("Register", &ShowRegister, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::InputText("Username", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::InputText("Password", InputBuf2, IM_ARRAYSIZE(InputBuf2), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password);
		if (ImGui::Button("Register"))
		{
			p->Register(InputBuf, InputBuf2);
		}
		ImGui::End();
	}
}
