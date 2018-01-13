#pragma once



class ServerApp :public Application
{
public:
	ServerApp();
	~ServerApp();
	
	virtual void RunMainLoop();
	virtual void RenderUI();

private:
	Log m_Logger;
};

