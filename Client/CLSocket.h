#pragma once

class CLSocket :	public Socket
{
public:
	CLSocket();
	~CLSocket();

	virtual bool	Connect(const char* name, const char* port = DEFAULT_PORT);
	SOCKET& Get() {
		return m_Socket;
	};
};

