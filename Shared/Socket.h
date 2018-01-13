#pragma once

#include <winsock2.h>

class Socket
{

public:
	Socket();
	Socket(SOCKET sk);
	~Socket();

	static bool InitWinShock();
	static bool ShutdownWinSock();

	bool	Send(const char* buffer, int len);
	int		Recv(char* buffer, int len);
	SOCKET	Get() { return m_Socket; }
	void	Close();
	int		Shutdown();

	virtual bool	Connect(const char* name = NULL, const char* port = DEFAULT_PORT) { return 0; };

private:
	static WSADATA wsaData;
protected:

	SOCKET m_Socket;
	
};