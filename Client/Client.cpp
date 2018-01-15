#include "stdafx.h"
#include "Client.h"


Client::~Client()
{
	
}


void Client::Update(float dt)
{

}

Client::Status Client::GetStatus()
{
	return m_Status;
}

void Client::SetStatus(Client::Status s)
{
	m_Status = s;
}