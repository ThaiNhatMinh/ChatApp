#pragma once



class Client: public Module
{
public:
	enum Status
	{

	};
public:
	Client()=default;
	~Client();
	
	bool Init();
	void Update(float dt);
	bool Register(const char* username, const char* pass);
	bool Login(const char* username, const char* pass);
	bool Send(const char* data,int len = DEFAULT_BUFLEN);

private:
	SOCKET ConnectSocket = INVALID_SOCKET;
	WSADATA wsaData;
};