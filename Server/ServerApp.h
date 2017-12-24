#pragma once



class ServerApp :public Application
{
public:
	ServerApp();
	~ServerApp();
	
	virtual void RunMainLoop();

};

