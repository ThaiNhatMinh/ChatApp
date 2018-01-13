#include "stdafx.h"
#include "Socket.h"

WSADATA Socket::wsaData;
Socket::Socket():m_Socket(INVALID_SOCKET)
{
}

Socket::Socket(SOCKET sk):m_Socket(sk)
{
}

Socket::~Socket()
{
}

bool Socket::InitWinShock()
{
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %s\n", gai_strerror(iResult));
		return 0;
	}
	return false;
}

bool Socket::ShutdownWinSock()
{
	WSACleanup();
	return false;
}

bool Socket::Send(const char * buffer, int len)
{
	int iResult = send(m_Socket, buffer, len, 0);

	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		return 0;
	}

	return 1;
}

int Socket::Recv(char * buffer, int len)
{
	int iResult = recv(m_Socket, buffer, len, 0);
	if (iResult == 0) printf("Connection has been close: %d\n", WSAGetLastError());
	else if (iResult < 0) printf("Socket Error: %d\n", WSAGetLastError());
	
	
	return iResult;
}

void Socket::Close()
{
	closesocket(m_Socket);
}

int Socket::Shutdown()
{
	return shutdown(m_Socket, SD_SEND);
}
