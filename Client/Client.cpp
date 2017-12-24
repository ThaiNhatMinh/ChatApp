#include "stdafx.h"
#include "Client.h"


Client::~Client()
{
	// shutdown the connection since no more data will be sent
	int iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
	}
	closesocket(ConnectSocket);
	WSACleanup();
}
bool Client::Init()
{
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 0;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 0;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 0;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 0;
	}
	
	return 1;
}

void Client::Update(float dt)
{
}

bool Client::Register(const char * username, const char * pass)
{
	char data[DEFAULT_BUFLEN];

	Buffer bf(data,DEFAULT_BUFLEN);
	char type = CMD_REG_USER;
	bf.WriteChar(&type, 1);

	bf.WriteInt(strlen(username) + 1);
	bf.WriteChar(username, strlen(username) + 1);

	bf.WriteInt(strlen(pass) + 1);
	bf.WriteChar(pass, strlen(pass) + 1);

	Send(bf.Get(),bf.GetPos());


	return false;
}

bool Client::Login(const char * username, const char * pass)
{
	return false;
}

bool Client::Send(const char * data,int len)
{
	// Send an initial buffer
	int iResult = send(ConnectSocket, data, len, 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 0;
	}

	printf("Bytes Sent: %ld\n", iResult);

	return 1;
}
Client::Status Client::GetStatus()
{
	return m_Status;
}

void Client::SetAsynch(HWND hwnd)
{
	WSAAsyncSelect(ConnectSocket, hwnd, WM_SOCKET,  FD_READ | FD_CLOSE);
}
void Client::SetStatus(Client::Status s)
{
	m_Status = s;
}