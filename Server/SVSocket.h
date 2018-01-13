#pragma once

class SVSocket :public Socket
{
public:
	SVSocket();
	~SVSocket();

	virtual bool	Connect(const char* name=NULL, const char* port = DEFAULT_PORT);
};

