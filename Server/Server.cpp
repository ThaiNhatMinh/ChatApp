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
	
	return 1;
}

void Server::Update()
{
	int iResult;
	//int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	
	
		for (size_t i = 0; i < m_Client.size(); i++)
		{

			iResult = recv(m_Client[i], recvbuf, recvbuflen, 0);

			if (iResult > 0) {
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
				printf("Client disconnect.");
				DisconnectClient(m_Client[i]);
			}
			else {
				printf("recv failed with error: %d\n", WSAGetLastError());
				//closesocket(m_Client[i]);
				//WSACleanup();
			}
		}

	
}

void Server::DisconnectClient(SOCKET client)
{
	// shutdown the connection since we're done
	int iResult = shutdown(client, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(client);
		WSACleanup();
	}
}

void Server::AddClient(SOCKET client)
{
	m_Client.push_back(client);
}
