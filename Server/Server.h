#pragma once
class Server: public Module
{
public:
	Server();
	~Server();
	bool Init();
	void Update(float dt);

	void DisconnectClient(SOCKET client);
	void ProcessEvent(WPARAM wParam, LPARAM lParam);
	void ProcessCommand(const char* cmd,int len);
	void SetAsynch(HWND hwnd);
private:
	SOCKET GetListenSocket() { return ListenSocket; }
	void AddClient(SOCKET client);
private:
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;

	std::vector<SOCKET> m_Client;
	std::unique_ptr<ThreadRAII> m_AddClientThread;
};

