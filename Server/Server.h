#pragma once
class Server
{
public:
	Server();
	~Server();
	bool Init();
	void Loop();
private:
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;

	std::vector<SOCKET> m_Client;
};

