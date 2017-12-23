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
	void ProcessCommand(char* cmd,int len, SOCKET sk);
	void SetAsynch(HWND hwnd);
private:

	// Read/Write user data
	void LoadUser();
	void WriteUser();

	SOCKET	GetListenSocket() { return ListenSocket; }
	
	// Check if username exist
	// True if exits
	bool	CheckUser(const char* username);

	// Find a user on server with socket/username
	// Return -1 if not found
	int		FindClient(SOCKET sk);
	int		FindClient(const char* username);

	// Send a message to Client/Socket
	bool	SendClient(Client*,Command, const char* text);
	bool	SendClient(SOCKET sk, Command, const char* text);

private:
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;

	std::vector<std::unique_ptr<Client>> m_Clients;
	std::vector<SOCKET> m_Connecting;
	std::unique_ptr<ThreadRAII> m_AddClientThread;
};

