#pragma once

struct SVFileSendInfo :public FileSendInfo
{
	char type;
	std::string name;
	std::string From;
};

struct Connection {
	Socket sd;
	char acBuffer[MAX_BUFFER_LEN];
	int nCharsInBuffer;

	Connection(SOCKET sd_) : sd(sd_), nCharsInBuffer(0) { }
};
typedef std::vector<Connection> ConnectionList;

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
	bool ReadData(Connection& conn);
	bool WriteData(Connection& conn);
	void TestSendFile(std::string file, Client*);
	void Send2ND(Client*);
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

	bool	SendFile(Client* cl);

private:
	Log& Logger;
	SVSocket m_SVSocket;
	std::vector<std::unique_ptr<Client>> m_Clients;
	std::vector<std::unique_ptr<GroupChatSV>> m_Groups;
	// Current connecting to server
	std::vector<Connection> m_Connecting;
	// check file sending -1 for none, 0 for sending, 1 for finish
	int FileSending;
	SVFileSendInfo m_File;
};

