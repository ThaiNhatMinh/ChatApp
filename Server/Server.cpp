// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"



Server::Server()
{
	
}


Server::~Server()
{
	
	for (size_t i = 0; i < m_Client.size(); i++)
	{
		DisconnectClient(m_Client[i]);
	}

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
	// shutdown the connection since we're done
	int iResult = shutdown(client, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());	
	}

	closesocket(client);
	auto it = std::find(m_Client.begin(), m_Client.end(), client);
	if(it!=m_Client.end()) m_Client.erase(it);
}

void Server::ProcessEvent(WPARAM wParam, LPARAM lParam)
{
	static int iResult;
	static char recvbuf[DEFAULT_BUFLEN];
	static int recvbuflen = DEFAULT_BUFLEN;
	if (WSAGETSELECTERROR(lParam))
	{
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(wParam);
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
			printf("Accept client.\n");
			AddClient(ClientSocket);
		}
		break;
	}
	case FD_READ:
	{
		iResult = recv(wParam, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			ProcessCommand(recvbuf, iResult);
			recvbuf[iResult] = '\0';
			printf("Received: %s\n", recvbuf);
			printf("Num byte: %d\n", iResult);

			/*
			iSendResult = send(m_Client[i], recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(m_Client[i]);
			WSACleanup();
			}else printf("Bytes sent: %d\n", iSendResult);*/
		}
		else if (iResult == 0)
		{
			printf("Client disconnect.\n");
			//DisconnectClient(m_Client[i]);
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			//closesocket(m_Client[i]);
			//WSACleanup();
		}
		break;
	}
	case FD_CLOSE:
	{
		DisconnectClient(wParam);
	}
	}
}

void Server::ProcessCommand(const char * cmd, int len)
{
	Command type = (Command)(cmd[0] - '0');
	switch (type)
	{
	case CMD_REG_USER:

		break;
	case CMD_REG_GROUP:
		break;
	case CMD_SEND_USER:
		break;
	case CMD_SEND_GROUP:
		break;
	case CMD_SEND_FILES:
		break;
	case CMD_RECV:
		break;
	case CMD_COUNT:
		break;
	default:
		break;
	}
}

void Server::SetAsynch(HWND hwnd)
{
	WSAAsyncSelect(ListenSocket, hwnd, WM_SOCKET, FD_ACCEPT | FD_READ | FD_CLOSE);
}

void Server::AddClient(SOCKET client)
{
	m_Client.push_back(client);
}
