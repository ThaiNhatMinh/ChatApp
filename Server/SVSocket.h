#pragma once

struct SocketEvent
{
	SOCKET m_Socket;
	unsigned int Flags;
	SocketEvent() :m_Socket(INVALID_SOCKET), Flags(0) {}
};
class SVSocket :public Socket
{
public:
	SVSocket();
	~SVSocket();

	virtual bool	Connect(const char* name=NULL, const char* port = DEFAULT_PORT);

	SocketEvent WaitEvent();
private:
	DWORD EventTotal = 0;

	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
	std::vector<SOCKET> m_SocketList;
};

