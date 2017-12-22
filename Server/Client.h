#pragma once


class Client
{

public:
	enum Status
	{
		S_CONNECTED,
		S_DISCONNECT,
		S_UNKNOW,

	};
	Client(SOCKET );
	~Client();

	bool Send(const char* data);

private:
	SOCKET m_Socket;
	Status m_Status;
};

