#pragma once

class CLSocket :	public Socket
{
public:
	CLSocket();
	~CLSocket();

	bool Send();
	virtual int		Send(const char* buffer, long len);
	virtual bool	Connect(const char* name, const char* port = DEFAULT_PORT);
	SOCKET& Get() {
		return m_Socket;
	};
};

