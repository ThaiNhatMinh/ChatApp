#pragma once
//#include "imgui\imgui.h"
#include "CLSocket.h"
#include "Client.h"
struct Messenger
{
	std::string Text;
	std::string Name;
};
struct UserInfo
{
	std::string Name;
	ImVec4 Color;
};
class ChatDiaglog
{
protected:
	std::vector<Messenger>		m_ChatData;
	bool						ScrollToBottom;
	bool						m_Open;
	std::string					m_Title;
	char						InputBuf[256];
	std::vector<UserInfo>		m_UserList;
	Client*						m_Client;
	CLSocket&					m_Socket;

protected:
	ImVec4 GetColor(const std::string& username);
public:
	ChatDiaglog(const char* name, Client* cl,CLSocket& sk) :m_Title(name), m_Open(1), m_Client(cl), m_Socket(sk){};
	~ChatDiaglog() = default;

	void			Clear();

	std::string&	GetName();
	bool			IsOpen();
	void			Open(bool a);
	void			AddMessenger(const char* user, const char* text);

	void			AddUser(std::string name);;
	virtual void	Draw();
};