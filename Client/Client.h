#pragma once



class Client: public Module
{
public:
	enum Status
	{
		NONE,
		LOGIN_FAIL,
		LOGIN_SUCCESS,
		REGISTER_FAIL,
		REGISTER_SUCCESS
	};
public:
	Client()=default;
	~Client();
	
	bool Init();
	void Update(float dt);
	bool Register(const char* username, const char* pass);
	bool Login(const char* username, const char* pass);
	bool Send(const char* data,int len = DEFAULT_BUFLEN);
	Client::Status GetStatus();
	void SetStatus(Client::Status);
	void SetAsynch(HWND hwnd);
	

private:
	SOCKET ConnectSocket = INVALID_SOCKET;
	WSADATA wsaData;
	Status m_Status;
};