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
private:
	bool ShowLogin;
	bool ShowRegister;
	bool ShowChat;

	Client* pClient;
	CLSocket m_CLSocket;
	std::vector<std::unique_ptr<ChatDiaglog>> m_ChatList;
};

