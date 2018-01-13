#include "stdafx.h"
#include "SVSocket.h"


SVSocket::SVSocket()
{
}


SVSocket::~SVSocket()
{
}

bool SVSocket::Connect(const char * name, const char * port)
{
	// address info for the server to listen to
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// set address information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	int iResult = getaddrinfo(name, port, &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		Socket::ShutdownWinSock();
		exit(1);
	}
	// Create a SOCKET for connecting to serve
	m_Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (m_Socket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		Socket::ShutdownWinSock();
		return 0;
	}

	// Setup the TCP listening socket
	iResult = bind(m_Socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(m_Socket);
		Socket::ShutdownWinSock();
		return 0;
	}

	freeaddrinfo(result);

	iResult = listen(m_Socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(m_Socket);
		Socket::ShutdownWinSock();
		return 0;
	}
	return 1;
}
