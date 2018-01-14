#pragma once


class Client
{

public:
	enum Status
	{
		OFFLINE,
		ONLINE

	};
	Client(SOCKET ,const char* user,const char* pass);
	~Client();


	void SetStatus(Client::Status st);
	Client::Status GetStatus();

	void SetSocket(SOCKET sk);
	Socket& GetSocket();

	const std::string& GetUsername();
	const std::string& GetPassWord();
private:
	Socket m_Socket;
	Status m_Status;
	std::string m_Username, m_Password;
};

