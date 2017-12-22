#pragma once



class Client
{
public:
	enum Status
	{

	};
public:
	Client()=default;
	~Client();

	bool Init();
	bool Register(const char* username, const char* pass);
	bool Login(const char* username, const char* pass);
	bool Send(const char* data);

private:
	SOCKET ConnectSocket = INVALID_SOCKET;
	WSADATA wsaData;
};