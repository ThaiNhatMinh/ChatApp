// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"



Server::Server()
{
	LoadUser();
}


Server::~Server()
{
	WriteUser();
	closesocket(ListenSocket);
	WSACleanup();
}


bool Server::Init()
{
	int iResult;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %s\n", gai_strerror(iResult));
		return 0;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %s\n", gai_strerror(iResult));
		WSACleanup();
		return 0;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 0;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 0;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 0;
	}


	/*
	// Thread to accept client
	auto ThreadFunc = [](Server* sv)
	{
		SOCKET ListenSocket = sv->GetListenSocket();
		while (1)
		{
			// Accept a client socket
			SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
			if (ClientSocket == INVALID_SOCKET) {
				printf("accept failed with error: %d\n", WSAGetLastError());
			}
			else
			{
				printf("Accept client:\n");
				sv->AddClient(ClientSocket);
			}
		}
	};

	// Create to wait any connect from client
	m_AddClientThread = std::make_unique<ThreadRAII>(ThreadFunc, this);
	*/

	
	return 1;
}

void Server::Update(float dt)
{
		
}

void Server::DisconnectClient(SOCKET client)
{
	int ID = FindClient(client);
	if (ID == -1)
	{
		// shutdown the connection since we're done
		int iResult = shutdown(client, SD_SEND);

		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
		}

		closesocket(client);
		auto it = std::find(m_Connecting.begin(), m_Connecting.end(), client);
		if (it != m_Connecting.end()) m_Connecting.erase(it);

	}
	else
	{
		Client* cl = m_Clients[ID].get();
		cl->SetStatus(Client::OFFLINE);
	}
}

void Server::ProcessEvent(WPARAM wParam, LPARAM lParam)
{
	static int iResult;
	static char recvbuf[DEFAULT_BUFLEN];
	static int recvbuflen = DEFAULT_BUFLEN;
	if (WSAGETSELECTERROR(lParam))
	{
		printf("Server::ProcessEvent() Accept failed with error: %d\n", WSAGetLastError());
		closesocket(wParam);
		return;
	}
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
	{
		// Accept a client socket
		SOCKET ClientSocket = accept(wParam, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
		}
		else
		{
			printf("Accept connecting.\n");
			m_Connecting.push_back(ClientSocket);
		}
		break;
	}
	case FD_READ:
	{
		iResult = recv(wParam, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			ProcessCommand(recvbuf, iResult, wParam);
			//recvbuf[iResult] = '\0';
			//printf("Received: %s\n", recvbuf);
			printf("Reive from %d. Num byte: %d\n", wParam, iResult);

			/*
			iSendResult = send(m_Clients[i], recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(m_Clients[i]);
			WSACleanup();
			}else printf("Bytes sent: %d\n", iSendResult);*/
		}
		else if (iResult == 0)
		{
			printf("Client disconnect.\n");
			//DisconnectClient(m_Clients[i]);
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			//closesocket(m_Clients[i]);
			//WSACleanup();
		}
		break;
	}
	case FD_CLOSE:
	{
		DisconnectClient(wParam);
		break;
	}
	}
}

void Server::ProcessCommand(char * cmd, int len,SOCKET sk)
{
	

	Buffer bf(cmd, len);

	char* a = bf.ReadChar();
	bf.IncPos(1);
	Command type = (Command)(*a);

	switch (type)
	{
	case CMD_REG_USER:
	{
		char username[MAX_USERNAME_LEN]; int unlen;
		char password[MAX_USERNAME_LEN]; int pwlen;

		// Read username and pass word from buffer
		unlen = bf.ReadInt();
		memcpy(username, bf.ReadChar(), unlen);
		bf.IncPos(unlen);
		pwlen = bf.ReadInt();
		memcpy(password, bf.ReadChar(), pwlen);
		bf.IncPos(pwlen);
		if (!CheckUser(username))
		{
			m_Clients.push_back(std::make_unique<Client>(sk, username, password));
			SendClient(sk,CMD_REG_SUCCESS, "Register user success!");
#if defined(_DEBUG)||defined(DEBUG)
			printf("Register success: %s %s\n", username, password);
#endif
		}
		else
		{
			SendClient(sk,CMD_REG_FAIL, "Register failer. Username has exits!");
		}
		break;
	}
	case CMD_REG_GROUP:
		break;
	case CMD_SEND_USER:
	{
		Client* cl = m_Clients[FindClient(sk)].get();
		char username[MAX_USERNAME_LEN]; int unlen;
		
		// Read username 
		unlen = bf.ReadInt();
		memcpy(username, bf.ReadChar(), unlen);

		int TargetID = FindClient(username);
		if (TargetID == -1)
		{
			SendClient(cl,CMD_SEND_USER_FAIL, "Can't found username.");
		}
		else
		{
			Client* Target = m_Clients[TargetID].get();
			if (Target->GetStatus() == Client::OFFLINE)
			{
				SendClient(cl, CMD_SEND_USER_FAIL, "User not online.");
			}
			else if(Target->GetStatus() == Client::ONLINE)
			{
				char messenger[DEFAULT_BUFLEN]; int textlen;
				textlen = bf.ReadInt();
				memcpy(messenger, bf.ReadChar(), textlen);
				SendClient(Target, CMD_SEND_USER, messenger);
			}
		}
		break;
	}
	case CMD_SEND_GROUP:
		break;
	case CMD_SEND_FILES:
		break;
	case CMD_RECV:
		break;
	case CMD_COUNT:
		break;
	default:
		printf("Unknow command.\n ");
		break;
	}
}

void Server::SetAsynch(HWND hwnd)
{
	WSAAsyncSelect(ListenSocket, hwnd, WM_SOCKET, FD_ACCEPT | FD_READ | FD_CLOSE);
}

void Server::LoadUser()
{
	std::ifstream fp("Data/data.bin");
	if (!fp)
	{
		std::cout << "Error: Can't load user data.\n";
		return;
	}
	std::string un, pw;
	while (fp >> un >> pw)
	{
		m_Clients.push_back(std::make_unique<Client>(INVALID_SOCKET, un.c_str(), pw.c_str()));
	}

	fp.close();
}

void Server::WriteUser()
{
	std::ofstream fp("Data/data.bin");
	if (!fp)
	{
		std::cout << "Error: Can't open user data.\n";
		return;
	}

	for(size_t i=0; i<m_Clients.size(); i++)
	{
		
		fp << m_Clients[i]->GetUsername() <<" "<< m_Clients[i]->GetPassWord() << std::endl;

	}

	fp.close();
}

bool Server::CheckUser(const char * username)
{
	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i]->GetUsername() == username) return true;
	}

	return false;
}

int Server::FindClient(SOCKET sk)
{
	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i]->GetSocket() == sk) return i;
	}

	return -1;
}

int Server::FindClient(const char * username)
{
	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i]->GetUsername() == username) return i;
	}

	return -1;
}

bool Server::SendClient(Client*cl, Command c, const char * text)
{
	return SendClient(cl->GetSocket(), c, text);
}

bool Server::SendClient(SOCKET sk, Command c, const char * text)
{
	char type = c;
	char data[DEFAULT_BUFLEN];
	Buffer bf(data, DEFAULT_BUFLEN);

	bf.WriteChar(&type, 1);
	bf.WriteChar(text, strlen(text) + 1);
	int iResult = send(sk, bf.Get(), DEFAULT_BUFLEN, 0);

	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		return 0;
	}

	return 1;
}

