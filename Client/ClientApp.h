#pragma once

class ClientApp :public Application
{
public:
	ClientApp();
	~ClientApp();

	void ProcessCommand(char * cmd, int len, SOCKET sk);

protected:
	virtual void RenderUI();

	bool ShowLogin;
	bool ShowRegister;
	bool ShowChat;
	Client* pClient;
};

