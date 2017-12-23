#pragma once

class ClientApp :public Application
{
public:
	ClientApp();
	~ClientApp();

protected:
	virtual void RenderUI();

	bool ShowLogin;
	bool ShowRegister;
	bool ShowChat;
	Client* p;
};

