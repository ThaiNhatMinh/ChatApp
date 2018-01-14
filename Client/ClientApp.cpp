#include "stdafx.h"
#include "ClientApp.h"


WNDPROC g_MainWndProc;
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR lpUserData = GetWindowLongPtr(hWnd, GWLP_USERDATA);
	static ClientApp* pMyObject = (ClientApp*)lpUserData;

	if (message == WM_SOCKET)
	{
		static int iResult;
		static char recvbuf[MAX_BUFFER_LEN];
		static int recvbuflen = MAX_BUFFER_LEN;
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
	Socket::InitWinShock();

	pClient = new Client();
	
	
	if (m_CLSocket.Connect("127.0.0.1") == false)
	{
		MessageBox(NULL, L"Cannot connect to server.", L"Error", 0);
		exit(0);
	}
	

	m_pModule = std::unique_ptr<Client>(pClient);

	HWND hWnd = m_Window->GetHandle();
	WSAAsyncSelect(m_CLSocket.Get(), hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	g_MainWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)MainWndProc);
	SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG)this);
	m_Window->ShowWindows();

}


ClientApp::~ClientApp()
{
	// shutdown the connection since no more data will be sent
	int iResult = m_CLSocket.Shutdown();
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
	}
	m_CLSocket.Close();

	Socket::ShutdownWinSock();
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
	{
		char username[MAX_USERNAME_LEN]; int unlen;

		unlen = bf.ReadInt();
		memcpy(username, bf.ReadChar(), unlen);
		bf.IncPos(unlen);
		int id = FindChatDiaglog(username);

		char text[MAX_BUFFER_LEN]; int textlen;
		textlen = bf.ReadInt();
		memcpy(text, bf.ReadChar(), textlen);
		bf.IncPos(textlen);
		if (id == -1)
		{
			m_ChatList.push_back(std::make_unique<ChatDiaglog>(username,pClient, m_CLSocket));
			m_ChatList.front()->AddMessenger(username, text);
			m_ChatList.front()->AddUser(username);

		}
		else
		{
			m_ChatList[id]->AddMessenger(username, text);
			// if not show then show it up
			if (!m_ChatList[id]->IsOpen()) m_ChatList[id]->Open(1);
		}

		printf("Messenger form %s id = %d\n", username, id);
		break;
	}
	case CMD_SEND_GROUP:
	{
		char grname[MAX_USERNAME_LEN]; int grlen;
		char msg[MAX_BUFFER_LEN]; int msglen;
		char username[MAX_USERNAME_LEN]; int unlen;

		grlen = bf.ReadInt();
		memcpy(grname, bf.ReadChar(), grlen);
		bf.IncPos(grlen);

		unlen = bf.ReadInt();
		memcpy(username, bf.ReadChar(), unlen);
		bf.IncPos(unlen);

		msglen = bf.ReadInt();
		memcpy(msg, bf.ReadChar(), msglen);
		bf.IncPos(msglen);

		int ID = FindGroupChat(grname);
		if (ID != -1)
		{
			m_GroupList[ID]->AddMessenger(username, msg);
		}
		else
		{
			m_GroupList.push_back(std::make_unique<GroupChat>(grname, pClient, m_CLSocket));
			m_GroupList.front()->AddMessenger(username, msg);
			m_GroupList.front()->AddUser(username);

			Buffer rqbf(MAX_BUFFER_LEN);
			rqbf.WriteChar(CMD_GET_GRINFO);
			rqbf.WriteInt(grlen);
			rqbf.WriteChar(grname, grlen);
			printf("[DEBUG] %s %d\n", grname, grlen);
			rqbf.WriteInt(pClient->GetUsername().size()+1);
			rqbf.WriteChar(pClient->GetUsername().c_str(), pClient->GetUsername().size() + 1);

			m_CLSocket.Send(rqbf.Get(), rqbf.GetPos());

		}
		break;
	}
	case CMD_GET_GRINFO:
	{
		char username[MAX_USERNAME_LEN]; int unlen;
		char grname[MAX_USERNAME_LEN]; int grlen;

		grlen = bf.ReadInt();
		memcpy(grname, bf.ReadChar(), grlen);
		bf.IncPos(grlen);

		int ID = FindGroupChat(grname);
		if (ID == -1) break;

		int num = bf.ReadInt();
		for (int i = 0; i < num; i++)
		{
			unlen = bf.ReadInt();
			memcpy(username, bf.ReadChar(), unlen);
			bf.IncPos(unlen);
			m_GroupList[ID]->AddUser(username);
		}
		break;
	}
	case CMD_SEND_FILES:
		break;
	
	case CMD_STATUS:
	{
		Status s = (Status)*bf.ReadChar();
		bf.IncPos(1);
		switch (s)
		{
		case SEND_TEXT:
		{
			break;
		}
		case SEND_FILE:
			break;
		case REG_USER:
		{
			char result = *bf.ReadChar();
			bf.IncPos(1);
			if (result == 0) pClient->SetStatus(Client::REGISTER_FAIL);
			else if (result == 1) pClient->SetStatus(Client::REGISTER_SUCCESS);
			else printf("Error: Messenger coprrupt.\n");

			break;
		}
		case LOGIN_USER:
		{
			char result = *bf.ReadChar();
			bf.IncPos(1);
			if (result == 0)
			{
				pClient->SetStatus(Client::LOGIN_FAIL);
			
			}
			else if (result == 1)
			{
				pClient->SetStatus(Client::LOGIN_SUCCESS);
			}
			else printf("Error: Messenger coprrupt.\n");
			break;
		}
		case ADD_USER:
		{
			char result = *bf.ReadChar();
			bf.IncPos(1);
			char grname[MAX_USERNAME_LEN]; int len;
			len = bf.ReadInt();
			memcpy(grname, bf.ReadChar(), len);
			bf.IncPos(len);
			int ID = FindGroupChat(grname);
			if (ID != -1)
			{
				m_GroupList[ID]->Status(result);
			}
			else
			{
				printf("Error: Invaid group name %s\n", grname);
			}
			break;
		}
		case REG_GROUP:
		{
			char result = *bf.ReadChar();
			bf.IncPos(1);
			char grname[MAX_USERNAME_LEN]; int len;
			len = bf.ReadInt();
			memcpy(grname, bf.ReadChar(), len);

			if (result == 0)
			{
				pClient->SetStatus(Client::REGISTER_GR_FAIL);

			}
			else if (result == 1)
			{
				pClient->SetStatus(Client::REGISTER_GR_SUCCESS);
				m_GroupList.push_back(std::make_unique<GroupChat>(grname, pClient, m_CLSocket));
				m_GroupList.front()->AddUser(pClient->GetUsername());
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	/*case CMD_REG_FAIL:
		pClient->SetStatus(Client::REGISTER_FAIL);
		break;
	case CMD_SEND_USER_SUCCESS:
		break;
	case CMD_SEND_USER_FAIL:
		break;
		*/
	
	default:
		break;
	}
}

void ClientApp::RenderUI()
{
	m_UI->NewFrame();

	
	
	if (ShowLogin)
	{
		static char InputBuf[MAX_USERNAME_LEN];
		static char InputBuf2[MAX_USERNAME_LEN];

		
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		ImGui::Begin("Login", &ShowLogin, ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoResize| ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoCollapse);
		ImGui::InputText("Username", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::InputText("Password", InputBuf2, IM_ARRAYSIZE(InputBuf2), ImGuiInputTextFlags_EnterReturnsTrue| ImGuiInputTextFlags_Password);
		if (ImGui::Button("Login") && (strlen(InputBuf) && strlen(InputBuf2)))
		{
			
			Buffer bf(MAX_BUFFER_LEN);
			bf.WriteChar(CMD_LOGIN_USER);
			bf.WriteInt(strlen(InputBuf) + 1);
			bf.WriteChar(InputBuf, strlen(InputBuf) + 1);

			bf.WriteInt(strlen(InputBuf2) + 1);
			bf.WriteChar(InputBuf2, strlen(InputBuf2) + 1);

			m_CLSocket.Send(bf.Get(), bf.GetPos());
			
		}
		if (pClient->GetStatus() == Client::LOGIN_FAIL) ImGui::OpenPopup("MessageBoxLoginF");
		else if (pClient->GetStatus() == Client::LOGIN_SUCCESS)
		{
			ShowLogin = 0;
			ShowChat = 1;
			pClient->GetUsername() = InputBuf;
			pClient->GetPassword() = InputBuf2;
		}
		if (ImGui::Button("Register"))
		{
			ShowRegister = !ShowRegister;
		}

		if (ImGui::BeginPopupModal("MessageBoxLoginF", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Login Fail!");
			if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
			pClient->SetStatus(Client::NONE);
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
			//pClient->Register(InputBuf, InputBuf2);
			Buffer bf(MAX_BUFFER_LEN);
			char type = CMD_REG_USER;
			bf.WriteChar(type);

			bf.WriteInt(strlen(InputBuf) + 1);
			bf.WriteChar(InputBuf, strlen(InputBuf) + 1);

			bf.WriteInt(strlen(InputBuf2) + 1);
			bf.WriteChar(InputBuf2, strlen(InputBuf2) + 1);

			m_CLSocket.Send(bf.Get(), bf.GetPos());
		}

		if (pClient->GetStatus() == Client::REGISTER_SUCCESS) ImGui::OpenPopup("MessageBoxRegS");
		else if (pClient->GetStatus() == Client::REGISTER_FAIL) ImGui::OpenPopup("MessageBoxRegF");
		if(ImGui::BeginPopupModal("MessageBoxRegS", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
		
		if (ImGui::BeginPopupModal("MessageBoxRegF", NULL, ImGuiWindowFlags_AlwaysAutoResize)) 
		{
			ImGui::Text("Register Fail!");
			if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
			pClient->SetStatus(Client::NONE);
		}
		ImGui::End();
	}


	if (ShowChat)
	{
		ImGui::Begin("Demo",nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{
			if (ImGui::Button("Send Messenger")) ImGui::OpenPopup("Input Username");
			if (ImGui::BeginPopupModal("Input Username",nullptr, ImGuiWindowFlags_NoResize))
			{
				static char un[MAX_USERNAME_LEN];
				if (ImGui::InputText("UserName", un, MAX_USERNAME_LEN, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					m_ChatList.push_back(std::make_unique<ChatDiaglog>(un, pClient,m_CLSocket));
					m_ChatList.front()->AddUser(un);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Button("OK", ImVec2(120, 0))) 
				{
					if(strlen(un)) m_ChatList.push_back(std::make_unique<ChatDiaglog>(un, pClient, m_CLSocket));
					m_ChatList.front()->AddUser(un);
					ImGui::CloseCurrentPopup(); 
				}
				ImGui::EndPopup();
			}


			// Create group
			{
				if (ImGui::Button("Create Group")) ImGui::OpenPopup("CreateGroup");
				if (ImGui::BeginPopupModal("CreateGroup", nullptr, ImGuiWindowFlags_NoResize))
			{
				static char un[MAX_USERNAME_LEN];
				if (ImGui::InputText("Name", un, MAX_USERNAME_LEN, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					// Request create group
					Buffer bf(MAX_BUFFER_LEN);
					bf.WriteChar(CMD_REG_GROUP);
					bf.WriteInt(strlen(un) + 1);
					bf.WriteChar(un, strlen(un) + 1);
					bf.WriteInt(pClient->GetUsername().size() + 1);
					bf.WriteChar(pClient->GetUsername().c_str(), pClient->GetUsername().size() + 1);

					m_CLSocket.Send(bf.Get(), bf.GetPos());

					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Button("OK", ImVec2(120, 0)) && strlen(un))
				{
					// Request create group
					Buffer bf(MAX_BUFFER_LEN);
					bf.WriteChar(CMD_REG_GROUP);
					bf.WriteInt(strlen(un) + 1);
					bf.WriteChar(un, strlen(un) + 1);
					bf.WriteInt(pClient->GetUsername().size() + 1);
					bf.WriteChar(pClient->GetUsername().c_str(), pClient->GetUsername().size() + 1);

					m_CLSocket.Send(bf.Get(), bf.GetPos());
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

				if (pClient->GetStatus() == Client::REGISTER_GR_FAIL) ImGui::OpenPopup("CreateGroupF");
				else if (pClient->GetStatus() == Client::REGISTER_GR_SUCCESS)
				{
					pClient->SetStatus(Client::NONE);
				}
				if (ImGui::BeginPopupModal("CreateGroupF", nullptr, ImGuiWindowFlags_NoResize))
				{
					ImGui::Text("Create Group Fail!");
					if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
					ImGui::EndPopup();
					pClient->SetStatus(Client::NONE);
				}
			}
		}
		ImGui::End();


		for (auto& el : m_ChatList)
		{
			el->Draw();
		}
		for (auto& el : m_GroupList)
		{
			el->Draw();
		}
	}
}

int ClientApp::FindChatDiaglog(const char * un)
{
	int i = 0;
	for (auto& el : m_ChatList)
	{
		if (el->GetName() == un) return i;
		i++;
	}
	return -1;
}

int ClientApp::FindGroupChat(const char * un)
{
	int i = 0;
	for (auto& el : m_GroupList)
	{
		if (el->GetName() == un) return i;
		i++;
	}
	return -1;
}

bool ClientApp::SendFile(const char * filename)
{
	FILE* pFile = fopen(filename, "rb");
	if (!pFile) return false;
	fseek(pFile, 0, SEEK_END);
	auto size = ftell(pFile);
	fseek(pFIle, 0, SEEK_SET);

	char* data = new char[size];
	fread(data, size, 1, pFile);
	fclose(pFile);


}
