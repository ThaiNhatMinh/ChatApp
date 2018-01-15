#include "stdafx.h"
#include "CLSocket.h"


CLSocket::CLSocket()
{
}


CLSocket::~CLSocket()
{
}

bool CLSocket::Send()
{
	int iResult = send(m_Socket, buffer, current, 0);

	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		return SOCKET_ERROR;
	}
	current = 0;
	return iResult;

}
int CLSocket::Send(const char * buffer, long len)
{
	memcpy(this->buffer, buffer, len);
	current = 0;
	return 1;
}
bool CLSocket::Connect(const char * name, const char * port)
{
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	int iResult;



	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(name, port, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 0;
	}
	
	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		m_Socket = socket(ptr->ai_family, ptr->ai_socktype,	ptr->ai_protocol);
		if (m_Socket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			Socket::ShutdownWinSock();
			return 0;
		}

		// Connect to server.
		iResult = connect(m_Socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (m_Socket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 0;
	}
	ULONG NonBlock = 1;

	if (ioctlsocket(m_Socket, FIONBIO, &NonBlock) == SOCKET_ERROR)

	{

		printf("ioctlsocket() failed with error %d\n", WSAGetLastError());

		return 1;

	}
	else
		fprintf(stdout, "ioctlsocket() is OK!\n");

	

	return 1;
}
