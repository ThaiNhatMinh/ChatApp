#include "stdafx.h"
#include "GroupChatSV.h"
#include "Client.h"


bool GroupChatSV::FindUser(const char * name)
{
	for (auto& el : m_UserList)
	{
		if (el.Name == name) return 1;
	}
	return false;
}

GroupChatSV::GroupChatSV(const char* name, Client* admin):m_Title(name)
{
	UserInfo ui;
	ui.Name = admin->GetUsername();
	ui.flag = FLAG_ADMIN;
	ui.m_pClient = admin;
	m_UserList.push_back(ui);
}

GroupChatSV::~GroupChatSV()
{
}

bool GroupChatSV::AddUser(Client* cl)
{
	if (FindUser(cl->GetUsername().c_str())) return 0;
	
	UserInfo ui;
	ui.flag = 0;
	ui.m_pClient = cl;
	ui.Name = cl->GetUsername();

	m_UserList.push_back(ui);
	return 1;
}

bool GroupChatSV::SendMsg(const char* us,const char * msg)
{
	for (auto& el : m_UserList)
	{
		if (el.m_pClient->GetStatus() == Client::OFFLINE) continue;


		Buffer msgbf(MAX_BUFFER_LEN);
		msgbf.WriteChar(CMD_SEND_GROUP);

		msgbf.WriteInt(m_Title.size() + 1);
		msgbf.WriteChar(m_Title.c_str(), m_Title.size() + 1);

		msgbf.WriteInt(strlen(us) + 1);
		msgbf.WriteChar(us, strlen(us) + 1);

		msgbf.WriteInt(strlen(msg) + 1);
		msgbf.WriteChar(msg, strlen(msg) + 1);

		Socket& sk = el.m_pClient->GetSocket();
		sk.Send(msgbf.Get(), msgbf.GetPos());

	}

	return 1;
}
