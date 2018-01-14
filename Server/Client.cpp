#include "stdafx.h"
#include "Client.h"


Client::Client(SOCKET sk, const char* user, const char* pass):m_Socket(sk),m_Username(user),m_Password(pass)
{
}


Client::~Client()
{
	// shutdown the connection since we're done
	//int iResult = shutdown(m_Socket, SD_SEND);
	
	if (m_Socket.Shutdown() == SOCKET_ERROR) {
		printf("%s shutdown failed with error: %d\n",m_Username.c_str(), WSAGetLastError());
	}

	m_Socket.Close();
}

void Client::SetStatus(Client::Status st)
{
	m_Status = st;
}

Client::Status Client::GetStatus()
{
	return m_Status;
}

void Client::SetSocket(SOCKET sk)
{
	m_Socket = sk;
}

Socket& Client::GetSocket()
{
	return m_Socket;
}

const std::string & Client::GetUsername()
{
	return m_Username;
}

const std::string & Client::GetPassWord()
{
	return m_Password;
}

