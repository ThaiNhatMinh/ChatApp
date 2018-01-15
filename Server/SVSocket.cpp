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
		fprintf(stdout, "getaddrinfo failed with error: %d\n", iResult);
		Socket::ShutdownWinSock();
		exit(1);
	}
	// Create a SOCKET for connecting to serve
	m_Socket = WSASocket(result->ai_family, result->ai_socktype, result->ai_protocol,NULL,0, WSA_FLAG_OVERLAPPED);
	if (m_Socket == INVALID_SOCKET) {
		fprintf(stdout, "socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		Socket::ShutdownWinSock();
		return 0;
	}
	
	iResult = bind(m_Socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		fprintf(stdout, "bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(m_Socket);
		Socket::ShutdownWinSock();
		return 0;
	}

	freeaddrinfo(result);

	iResult = listen(m_Socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		fprintf(stdout, "listen failed with error: %d\n", WSAGetLastError());
		closesocket(m_Socket);
		Socket::ShutdownWinSock();
		return 0;
	}
	// Change the socket mode on the listening socket from blocking to

	// non-block so the application will not block waiting for requests

	ULONG NonBlock = 1;

	if (ioctlsocket(m_Socket, FIONBIO, &NonBlock) == SOCKET_ERROR)

	{

		printf("ioctlsocket() failed with error %d\n", WSAGetLastError());

		return 1;

	}else 
		fprintf(stdout,"ioctlsocket() is OK!\n");


	return 1;
}

SocketEvent SVSocket::WaitEvent()
{
	DWORD Event;
	WSANETWORKEVENTS NetworkEvents;
	SOCKET Accept;
	Event = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, 100, FALSE);
	if (Event == WSA_WAIT_FAILED)
	{
		fprintf(stdout,"WSAWaitForMultipleEvents() failed with error %d\n", WSAGetLastError());
		return SocketEvent();
	}else if (Event == WSA_WAIT_TIMEOUT)
	{
		return SocketEvent();
	}
	else fprintf(stdout, "WSAWaitForMultipleEvents() is pretty damn OK!\n");

	if (WSAEnumNetworkEvents(m_SocketList[Event - WSA_WAIT_EVENT_0], EventArray[Event - WSA_WAIT_EVENT_0], &NetworkEvents) == SOCKET_ERROR)
	{
		fprintf(stdout, "WSAEnumNetworkEvents() failed with error %d\n", WSAGetLastError());
		return SocketEvent();
	}
	else fprintf(stdout, "WSAEnumNetworkEvents() should be fine!\n");

	if (NetworkEvents.lNetworkEvents & FD_ACCEPT)
	{

		if (NetworkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
		{
			fprintf(stdout," FD_ACCEPT failed with error %d\n", NetworkEvents.iErrorCode[FD_ACCEPT_BIT]);
			return SocketEvent();

		}



		if ((Accept = accept(m_Socket, NULL, NULL)) == INVALID_SOCKET)
		{
			fprintf(stdout," accept() failed with error %d\n", WSAGetLastError());
			return SocketEvent();
		}
		else	fprintf(stdout," accept() should be OK!\n");



		if (EventTotal > WSA_MAXIMUM_WAIT_EVENTS)
		{
			fprintf(stdout," Too many connections - closing socket...\n");
			closesocket(Accept);
			return SocketEvent();
		}



		if ((EventArray[EventTotal] = WSACreateEvent()) == WSA_INVALID_EVENT)
		{
			fprintf(stdout, "WSACreateEvent() failed with error %d\n", WSAGetLastError());
			return SocketEvent();
		}
		else
		{
			fprintf(stdout, "WSACreateEvent() is OK!\n");
			EventTotal++;
		}



		if (WSAEventSelect(Accept, EventArray[EventTotal - 1], FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
		{
			fprintf(stdout," WSAEventSelect() failed with error %d\n", WSAGetLastError());
			return SocketEvent();
		}
		else fprintf(stdout," WSAEventSelect() is OK!\n");

		fprintf(stdout, "Socket %d got connected...\n", Accept);

		SocketEvent a;
		a.m_Socket = Accept;
		a.Flags = FD_ACCEPT;
		return a;
	}



	// Try to read and write data to and from the data buffer if read and write events occur

	if (NetworkEvents.lNetworkEvents & FD_READ)
	{
		if (NetworkEvents.lNetworkEvents & FD_READ && NetworkEvents.iErrorCode[FD_READ_BIT] != 0)
		{
			fprintf(stdout," FD_READ failed with error %d\n", NetworkEvents.iErrorCode[FD_READ_BIT]);
			return SocketEvent();
		}else	fprintf(stdout," FD_READ is OK!\n");

		SOCKET sk = m_SocketList[Event - WSA_WAIT_EVENT_0];
		SocketEvent a;
		a.m_Socket = sk;
		a.Flags = FD_READ;
		return a;
		
	}

	if (NetworkEvents.lNetworkEvents & FD_WRITE)
	{
		if (NetworkEvents.lNetworkEvents & FD_WRITE && NetworkEvents.iErrorCode[FD_WRITE_BIT] != 0)
		{
			fprintf(stdout, " FD_WRITE failed with error %d\n", NetworkEvents.iErrorCode[FD_WRITE_BIT]);
			return SocketEvent();
		}
		else	fprintf(stdout, " FD_WRITE is OK!\n");

		SOCKET sk = m_SocketList[Event - WSA_WAIT_EVENT_0];
		SocketEvent a;
		a.m_Socket = sk;
		a.Flags = FD_WRITE;
		return a;
	}

	if (NetworkEvents.lNetworkEvents & FD_CLOSE)
	{
		if (NetworkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
		{
			fprintf(stdout, "FD_CLOSE failed with error %d\n", NetworkEvents.iErrorCode[FD_CLOSE_BIT]);
			return SocketEvent();
		}
		else	fprintf(stdout," FD_CLOSE is OK!\n");



		fprintf(stdout, "Closing socket information %d\n", m_SocketList[Event - WSA_WAIT_EVENT_0]);

		SocketEvent a;
		a.m_Socket = m_SocketList[Event - WSA_WAIT_EVENT_0];
		a.Flags = FD_CLOSE;
		return a;

	}
	
}
