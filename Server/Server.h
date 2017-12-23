#pragma once
class Server
{
public:
	Server();
	~Server();
	bool Init();
	void Update();

	void DisconnectClient(SOCKET client);

private:
	SOCKET GetListenSocket() { return ListenSocket; }
	void AddClient(SOCKET client);
private:
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;

	std::vector<SOCKET> m_Client;
	std::unique_ptr<ThreadRAII> m_AddClientThread;
};

