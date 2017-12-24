#include "stdafx.h"
#include "ClientApp.h"


WNDPROC g_MainWndProc;
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR lpUserData = GetWindowLongPtr(hWnd, GWLP_USERDATA);
	ClientApp* pMyObject = (ClientApp*)lpUserData;

	if (message == WM_SOCKET)
	{
		static int iResult;
		static char recvbuf[DEFAULT_BUFLEN];
		static int recvbuflen = DEFAULT_BUFLEN;
		if (WSAGETSELECTERROR(lParam))
		{
			printf("ClientApp::ProcessEvent() Accept failed with error: %d\n", WSAGetLastError());
			closesocket(wParam);
			return CallWindowProc(g_MainWndProc, hWnd, message, wParam, lParam);
		}
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
		{
			iResult = recv(wParam, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				pMyObject->ProcessCommand(recvbuf, iResult, wParam);
				printf("Reive from %d. Num byte: %d\n", wParam, iResult);
			}
			else if (iResult == 0)
			{
				printf("Server disconnect.\n");
				//DisconnectClient(m_Clients[i]);
			}
			else {
				printf("recv failed with error: %d\n", WSAGetLastError());
			}
			break;
		}
		case FD_CLOSE:
		{
			printf("Server disconnect.\n");
			break;
		}
		}
	}

	return CallWindowProc(g_MainWndProc, hWnd, message, wParam, lParam);
}

ClientApp::ClientApp():ShowLogin(1),ShowRegister(0),ShowChat(0)
{
	pClient = new Client();
	if (!pClient->Init())
	{
		printf("Cannot init client.");
		exit(0);
	}


	m_pModule = std::unique_ptr<Client>(pClient);

	HWND hWnd = m_Window->GetHandle();
	pClient->SetAsynch(hWnd);
	g_MainWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)MainWndProc);
	SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG)this);
	m_Window->ShowWindows();

}


ClientApp::~ClientApp()
{
}

void ClientApp::ProcessCommand(char * cmd, int len, SOCKET sk)
{
	Buffer bf(cmd, len);

	char* a = bf.ReadChar();
	bf.IncPos(1);
	Command type = (Command)(*a);
	switch (type)
	{
	case CMD_SEND_USER:
		break;
	case CMD_SEND_GROUP:
		break;
	case CMD_SEND_FILES:
		break;
	case CMD_RECV:
		break;
	case CMD_REG_SUCCESS:
		pClient->SetStatus(Client::REGISTER_SUCCESS);
		break;
	case CMD_REG_FAIL:
		pClient->SetStatus(Client::REGISTER_FAIL);
		break;
	case CMD_SEND_USER_SUCCESS:
		break;
	case CMD_SEND_USER_FAIL:
		break;
	case CMD_COUNT:
		break;
	default:
		break;
	}
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
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		ImGui::Begin("Login", &ShowLogin, ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoResize| ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoCollapse);
		ImGui::InputText("Username", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::InputText("Password", InputBuf2, IM_ARRAYSIZE(InputBuf2), ImGuiInputTextFlags_EnterReturnsTrue| ImGuiInputTextFlags_Password);
		if (ImGui::Button("Login") && (strlen(InputBuf) && strlen(InputBuf2)))
		{
			
			pClient->Login(InputBuf, InputBuf2);
			if (pClient->GetStatus() == Client::LOGIN_FAIL)
			{
				//ImGui::BeginChild("MessageBox", ImVec2(200, 100), true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				//ImGui::Text("Login Fail!");
				//ImGui::EndChild();
				MessageBox(m_Window->GetHandle(), L"Error", L"Login Fail!", 0);
				
			}
		}
		if (ImGui::Button("Register"))
		{
			ShowRegister = !ShowRegister;
		}
		ImGui::End();
	}
	if (ShowRegister)
	{
		static char InputBuf[MAX_USERNAME_LEN]; 
		static char InputBuf2[MAX_USERNAME_LEN]; 
		
		ImGui::SetNextWindowSize(ImVec2(250, 200), ImGuiCond_FirstUseEver);
		ImGui::Begin("Register", &ShowRegister, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::InputText("Username", InputBuf, IM_ARRAYSIZE(InputBuf));
		ImGui::InputText("Password", InputBuf2, IM_ARRAYSIZE(InputBuf2),ImGuiInputTextFlags_Password);
		if (ImGui::Button("Register") && (strlen(InputBuf) && strlen(InputBuf2)))
		{
			pClient->Register(InputBuf, InputBuf2);
			
		}
		if (pClient->GetStatus() == Client::REGISTER_SUCCESS) ImGui::OpenPopup("MessageBoxS");
		else if (pClient->GetStatus() == Client::REGISTER_FAIL) ImGui::OpenPopup("MessageBoxF");
		if(ImGui::BeginPopupModal("MessageBoxS", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Register Success!");
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{ 
				ImGui::CloseCurrentPopup(); 
				pClient->SetStatus(Client::NONE);
				ShowRegister = 0;
			}
			ImGui::EndPopup();

			
		}
		
		if (ImGui::BeginPopupModal("MessageBoxF", NULL, ImGuiWindowFlags_AlwaysAutoResize)) 
		{
			ImGui::Text("Register Fail!");
			if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
			pClient->SetStatus(Client::NONE);
		}
		ImGui::End();
	}
}
