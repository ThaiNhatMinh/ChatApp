#pragma once

class ClientApp :public Application
{
public:
	ClientApp();
	~ClientApp();

	void ProcessEvent(WPARAM wParam, LPARAM lParam);
	virtual void RunMainLoop();
protected:
	void ProcessCommand(char * cmd, int len, SOCKET sk);
	
	virtual void RenderUI();
	void ProcessSocket();
	int FindChatDiaglog(const char* un);
	int FindGroupChat(const char* un);
	void SaveFile();
private:
	bool ShowLogin;
	bool ShowRegister;
	bool ShowChat;

	Client* pClient;
	CLSocket m_CLSocket;

	std::vector<std::unique_ptr<ChatDiaglog>> m_ChatList;
	std::vector<std::unique_ptr<GroupChat>> m_GroupList;
	std::vector<std::unique_ptr<ThreadRAII>> m_FileThread;

	// check file sending -1 for none, 0 for sending, 1 for finish
	int FileSending;
	FileSendInfo m_File;
};

