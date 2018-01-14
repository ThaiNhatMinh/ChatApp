#pragma once
class Server: public Module
{
public:
	Server(Log& l);
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

	SOCKET	GetListenSocket() { return m_SVSocket.Get(); }
	
	// Check if username exist
	// True if exits
	bool	CheckUser(const char* username);

	// Find a user on server with socket/username
	// Return -1 if not found
	int		FindClient(SOCKET sk);
	int		FindClient(const char* username,const char* password=nullptr);
	int		FindGroup(const char* name);
	// Send a message to Client/Socket
	bool	SendClient(Client*, const char * buffer, int len);
	bool	SendClient(SOCKET sk, const char* buffer,int len);

private:
	Log& Logger;
	SVSocket m_SVSocket;
	std::vector<std::unique_ptr<Client>> m_Clients;
	std::vector<std::unique_ptr<GroupChatSV>> m_Groups;
	// Current connecting to server
	std::vector<Socket> m_Connecting;
};

