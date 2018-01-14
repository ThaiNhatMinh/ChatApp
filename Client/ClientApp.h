#pragma once

class ClientApp :public Application
{
public:
	ClientApp();
	~ClientApp();

	void ProcessCommand(char * cmd, int len, SOCKET sk);

protected:
	virtual void RenderUI();
	int FindChatDiaglog(const char* un);
	int FindGroupChat(const char* un);
	bool SendFile(const char* filename);
private:
	bool ShowLogin;
	bool ShowRegister;
	bool ShowChat;

	Client* pClient;
	CLSocket m_CLSocket;

	std::vector<std::unique_ptr<ChatDiaglog>> m_ChatList;
	std::vector<std::unique_ptr<GroupChat>> m_GroupList;
};

