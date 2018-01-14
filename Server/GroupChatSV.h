#pragma once

#define FLAG_ADMIN  1<<1
class Client;
struct UserInfo
{
	std::string Name;
	Client* m_pClient;
	int flag;
};
class GroupChatSV
{
private:
	std::vector<UserInfo>	m_UserList;
	std::string				m_Title;
protected:
	bool FindUser(const char* name);
public:
	GroupChatSV(const char* title, Client* admin);
	~GroupChatSV();

	bool AddUser(Client* cl);

	bool SendMsg(const char* us, const char* msg);
	const std::string& GetName() { return m_Title; }
	const std::vector<UserInfo>& GetUserInfo() {
		return m_UserList;
	}
};

