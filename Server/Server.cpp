// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"



Server::Server(Log& l):Logger(l)
{
	LoadUser();
}


Server::~Server()
{
	WriteUser();
	
	m_SVSocket.Close();

	Socket::ShutdownWinSock();
}


bool Server::Init()
{
	

	FileSending = -1;
	return m_SVSocket.Connect();
}

bool Server::WriteData(Connection& conn)
{
	return 1;
	int nBytes = send(conn.sd.Get(), conn.acBuffer, conn.nCharsInBuffer, 0);
	if (nBytes == SOCKET_ERROR) {
		// Something bad happened on the socket.  Deal with it.
		int err;
		int errlen = sizeof(err);
		getsockopt(conn.sd.Get(), SOL_SOCKET, SO_ERROR, (char*)&err, &errlen);
		if (err == WSAEWOULDBLOCK) Logger.AddLog("[Info]WSAEWOULDBLOCK\n");
		return (err == WSAEWOULDBLOCK);
	}

	if (nBytes == conn.nCharsInBuffer) {
		// Everything got sent, so take a shortcut on clearing buffer.
		conn.nCharsInBuffer = 0;
	}
	else {
		// We sent part of the buffer's data.  Remove that data from
		// the buffer.
		conn.nCharsInBuffer -= nBytes;
		memmove(conn.acBuffer, conn.acBuffer + nBytes,
			conn.nCharsInBuffer);
	}

	return true;
}
bool Server::ReadData(Connection& conn)
{
	int nBytes = recv(conn.sd.Get(), conn.acBuffer,MAX_BUFFER_LEN , 0);
	if (nBytes == 0) 
	{
		Logger.AddLog("[Info] Socket %d was closed by the client. Shutting down.", conn.sd.Get());
		return false;
	}
	else if (nBytes == SOCKET_ERROR) {
		// Something bad happened on the socket.  It could just be a
		// "would block" notification, or it could be something more
		// serious.  Let caller handle the latter case.  WSAEWOULDBLOCK
		// can happen after select() says a socket is readable under
		// Win9x: it doesn't happen on WinNT/2000 or on Unix.
		int err;
		int errlen = sizeof(err);
		getsockopt(conn.sd.Get(), SOL_SOCKET, SO_ERROR, (char*)&err, &errlen);
		if(err == WSAEWOULDBLOCK) Logger.AddLog("[Info]WSAEWOULDBLOCK\n");
		return (err == WSAEWOULDBLOCK);
	}

	// We read some bytes.  Advance the buffer size counter.
	conn.nCharsInBuffer += nBytes;
	ProcessCommand(conn.acBuffer, nBytes, conn.sd.Get());
	return true;
}

void Server::Update(float dt)
{
	sockaddr_in sinRemote;
	int nAddrSize = sizeof(sinRemote);
	

	
	{
		fd_set ReadFDs, WriteFDs, ExceptFDs;
		FD_ZERO(&ReadFDs);
		FD_ZERO(&WriteFDs);
		FD_ZERO(&ExceptFDs);

		// Add the listener socket to the read and except FD sets, if there
		// is one.
		if (m_SVSocket.Get() != INVALID_SOCKET)
		{
			FD_SET(m_SVSocket.Get(), &ReadFDs);
			FD_SET(m_SVSocket.Get(), &ExceptFDs);
		}
		// Add client connections
		/*ConnectionList::iterator it = m_Connecting.begin();
		while (it != m_Connecting.end()) {
			if (it->nCharsInBuffer < MAX_BUFFER_LEN) {
				// There's space in the read buffer, so pay attention to
				// incoming data.
				FD_SET(it->sd.Get(), &ReadFDs);
			}

			if (it->nCharsInBuffer > 0) {
				// There's data still to be sent on this socket, so we need
				// to be signalled when it becomes writable.
				FD_SET(it->sd.Get(), &WriteFDs);
			}

			FD_SET(it->sd.Get(), &ExceptFDs);

			++it;
		}*/

		timeval time;
		time.tv_usec = 50;
		time.tv_sec = 0;
		if (select(0, &ReadFDs, &WriteFDs, &ExceptFDs, &time) > 0)
		{
			//// Something happened on one of the sockets.
			// Was it the listener socket?...
			if (FD_ISSET(m_SVSocket.Get(), &ReadFDs))
			{
				SOCKET sd = accept(m_SVSocket.Get(),(sockaddr*)&sinRemote, &nAddrSize);

				if (sd != INVALID_SOCKET) {
					// Tell user we accepted the socket, and add it to
					// our connecition list.
					Logger.AddLog("[Info] Accepted connection from %s : %d , socket %d .\n", inet_ntoa(sinRemote.sin_addr), ntohs(sinRemote.sin_port),sd);
					m_Connecting.push_back(Connection(sd));
					if ((m_Connecting.size() + 1) > 64) {
						// For the background on this check, see
						// www.tangentsoft.net/wskfaq/advanced.html#64sockets
						// The +1 is to account for the listener socket.
						Logger.AddLog("[Warning] More than 63 client connections accepted.  This will not work reliably on some Winsock stacks!\n");
					}

					// Mark the socket as non-blocking, for safety.
					u_long nNoBlock = 1;
					ioctlsocket(sd, FIONBIO, &nNoBlock);
				}
				else {
					Logger.AddLog("[Error] accept() failed\n");
					return;
				}
			}
			else if (FD_ISSET(m_SVSocket.Get(), &ExceptFDs)) {
				int err;
				int errlen = sizeof(err);
				getsockopt(m_SVSocket.Get(), SOL_SOCKET, SO_ERROR,	(char*)&err, &errlen);
				Logger.AddLog("[Error] Exception on listening socket : %s\n", (char*)err);
				return;
			}

			// ...Or was it one of the client sockets?
			ConnectionList::iterator it = m_Connecting.begin();
			while (it != m_Connecting.end()) {
				bool bOK = true;
				const char* pcErrorType = 0;

				// See if this socket's flag is set in any of the FD
				// sets.
				if (FD_ISSET(it->sd.Get(), &ExceptFDs)) {
					bOK = false;
					pcErrorType = "General socket error";
					FD_CLR(it->sd.Get(), &ExceptFDs);
				}
				else 
				{
					if (FD_ISSET(it->sd.Get(), &ReadFDs)) {
						Logger.AddLog("[Info] Socket %d became readable; handling it.\n", it->sd.Get());
						bOK = ReadData(*it);
						pcErrorType = "Read error";
						FD_CLR(it->sd.Get(), &ReadFDs);
					}
					if (FD_ISSET(it->sd.Get(), &WriteFDs)) {
						Logger.AddLog("[Info] Socket %d became writable; handling it.\n", it->sd.Get());
						bOK = WriteData(*it);
						pcErrorType = "Write error";
						FD_CLR(it->sd.Get(), &WriteFDs);
					}
				}

				if (!bOK) {
					// Something bad happened on the socket, or the
					// client closed its half of the connection.  Shut
					// the conn down and remove it from the list.
					int err;
					int errlen = sizeof(err);
					getsockopt(it->sd.Get(), SOL_SOCKET, SO_ERROR,
						(char*)&err, &errlen);
					if (err != NO_ERROR) {
						Logger.AddLog("[Error] DDASDS\n");
					}
					DisconnectClient(it->sd.Get());
					m_Connecting.erase(it);
					it = m_Connecting.begin();
				}
				else {
					// Go on to next connection
					++it;
				}
			}
		}
	}

	if (ImGui::Button("DDD"))
	{
		for (size_t i = 0; i < m_Clients.size(); i++)
		{
			if (m_Clients[i]->GetStatus() == Client::OFFLINE) continue;
			Logger.AddLog("[File] %d %s %d\n",i, m_Clients[i]->GetUsername().c_str(), m_Clients[i]->GetSocket().Get());
			TestSendFile("E:\\3004.jpg", m_Clients[i].get());
		}
	}
}

void Server::DisconnectClient(SOCKET client)
{
	int ID = FindClient(client);
	if (ID == -1)
	{
		// shutdown the connection since we're done
		int iResult = shutdown(client, SD_SEND);

		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
		}

		closesocket(client);
		for (auto it = m_Connecting.begin(); it!=m_Connecting.end(); it++)
		{
			if (it->sd.Get() == client)
			{
				m_Connecting.erase(it);
				break;
			}
		}
		Logger.AddLog("[Info] Client %d has been disconnect.", client);
		
	}
	else
	{
		Client* cl = m_Clients[ID].get();
		cl->SetStatus(Client::OFFLINE);
	}
}

void Server::ProcessEvent(WPARAM wParam, LPARAM lParam)
{
	static int iResult;
	static char recvbuf[MAX_BUFFER_LEN];
	static int recvbuflen = MAX_BUFFER_LEN;
	if (WSAGETSELECTERROR(lParam))
	{
		//printf("Server::ProcessEvent() Accept failed with error: %d\n", WSAGetLastError());
		Logger.AddLog("[Error] Server::ProcessEvent() Accept failed with error: %d\n", WSAGetLastError());
		closesocket(wParam);
		return;
	}
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
	{
		// Accept a client socket
		SOCKET ClientSocket = accept(wParam, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			Logger.AddLog("[Error] Accept failed with error: %d\n", WSAGetLastError());
		}
		else
		{
			Logger.AddLog("[Info] Accept connecting with socket: %d %d\n", wParam,m_SVSocket.Get());
			m_Connecting.push_back(Connection(ClientSocket));
		}
		break;
	}
	case FD_READ:
	{
		//Logger.AddLog("[Info] SSSS\n");
		if (FileSending == -1)
		{
			iResult = recv(wParam, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				
				ProcessCommand(recvbuf, iResult, wParam);
				//Logger.AddLog("[Info] Reive from %d. Num byte: %d\n", wParam, iResult);
				
			}
			else if (iResult == 0)
			{
				Logger.AddLog("[Info] Client %d disconnect.\n", wParam);
				//DisconnectClient(m_Clients[i]);
			}
			else {
				Logger.AddLog("[Error] Recv failed with error: %d\n", WSAGetLastError());
				//closesocket(m_Clients[i]);
				//WSACleanup();
			}
		}
		else if (FileSending == 0)
		{
			
			while (m_File.current < m_File.size)
			{
				iResult = recv(wParam, &m_File.pData[m_File.current], m_File.size - m_File.current, 0);
				if (iResult == SOCKET_ERROR)
				{
					printf("Read data from file error.");
					break;
				}
				else if (iResult == 0)
				{
					Logger.AddLog("[Info] Client %d disconnect.\n", wParam);
					//DisconnectClient(m_Clients[i]);
				}
				else
				{
					m_File.current += iResult;
				}
				Logger.AddLog("[File] Recv data: %d\n", iResult);
			}
			
			if (m_File.current == m_File.size)
			{
				Logger.AddLog("[File] Finish recv");
				/*FILE* pFile = fopen(m_File.FileName.c_str(), "wb");
				if (!pFile)
				{
					Logger.AddLog("[FILE] Cannot create file");
					break;
				}

				fwrite(m_File.pData, m_File.size, 1, pFile);
				fclose(pFile);*/
				if (m_File.type == 0)
				{
					int IDClient = FindClient(m_File.name.c_str());
					if (IDClient != -1 && m_Clients[IDClient]->GetStatus()!=Client::OFFLINE)
					{
						SendFile(m_Clients[IDClient].get());
					}
				}
				else if (m_File.type == 1)
				{
					int IDGroup = FindGroup(m_File.name.c_str());
					for (auto& el : m_Groups[IDGroup]->GetUserInfo())
					{
						if (el.Name == m_File.From) continue;
						if (el.m_pClient->GetStatus() != Client::OFFLINE) SendFile(el.m_pClient);
					}
				}
				delete[] m_File.pData;
				FileSending = -1;

			}
			else if( m_File.current >= m_File.size)
			{
				Logger.AddLog("[File] Error File size: %d|Current: %d", m_File.size, m_File.current);
				FileSending = -1;
			}

		}
		break;
	}
	case FD_CLOSE:
	{
		DisconnectClient(wParam);
		break;
	}
	}
}

void Server::ProcessCommand(char * cmd, int len,SOCKET sk)
{
	

	Buffer bf(cmd, len);

	char* a = bf.ReadChar();
	bf.IncPos(1);
	Command type = (Command)(*a);

	switch (type)
	{
	case CMD_REG_USER:
	{
		char username[MAX_USERNAME_LEN]; int unlen;
		char password[MAX_USERNAME_LEN]; int pwlen;

		// Read username and pass word from buffer
		unlen = bf.ReadInt();
		memcpy(username, bf.ReadChar(), unlen);
		bf.IncPos(unlen);
		pwlen = bf.ReadInt();
		memcpy(password, bf.ReadChar(), pwlen);
		bf.IncPos(pwlen);
		if (!CheckUser(username))
		{
			m_Clients.push_back(std::make_unique<Client>(sk, username, password));

			
			Buffer stbf(MAX_STATUS_LEN);
			stbf.WriteChar(CMD_STATUS);
			stbf.WriteChar(REG_USER);
			stbf.WriteChar(1);
			stbf.WriteInt(24);
			stbf.WriteChar("Register user success!", 24);

			SendClient(sk, stbf.Get(),stbf.GetPos());


			Logger.AddLog("[Info] Register success: %s %s\n", username, password);

		}
		else
		{
			Buffer stbf(MAX_STATUS_LEN);
			stbf.WriteChar(CMD_STATUS);
			stbf.WriteChar(REG_USER);
			stbf.WriteChar(0);
			stbf.WriteInt(38);
			stbf.WriteChar("Register failer. Username has exits!", 38);

			SendClient(sk, stbf.Get(), stbf.GetPos());
			Logger.AddLog("[Info] %d Register failed.\n", sk);
		}
		break;
	}
	case CMD_LOGIN_USER:
	{
		char username[MAX_USERNAME_LEN]; int unlen;
		char password[MAX_USERNAME_LEN]; int pwlen;

		// Read username and pass word from buffer
		unlen = bf.ReadInt();
		memcpy(username, bf.ReadChar(), unlen);
		bf.IncPos(unlen);
		pwlen = bf.ReadInt();
		memcpy(password, bf.ReadChar(), pwlen);
		bf.IncPos(pwlen);

		int id = FindClient(username, password);
		if (id == -1)
		{
			Buffer stbf(MAX_STATUS_LEN);
			stbf.WriteChar(CMD_STATUS);
			stbf.WriteChar(LOGIN_USER);
			stbf.WriteChar(0);
			stbf.WriteInt(18);
			stbf.WriteChar("Login user fail!", 18);

			SendClient(sk, stbf.Get(), stbf.GetPos());
		}
		else
		{
			Buffer stbf(MAX_STATUS_LEN);
			stbf.WriteChar(CMD_STATUS);
			stbf.WriteChar(LOGIN_USER);
			stbf.WriteChar(1);
			stbf.WriteInt(21);
			stbf.WriteChar("Login user success!", 21);
			SendClient(sk, stbf.Get(), stbf.GetPos());

			m_Clients[id]->SetStatus(Client::ONLINE);
			m_Clients[id]->SetSocket(sk);
			//Logger.AddLog("[File] %d %s %d\n", id,m_Clients[id]->GetUsername().c_str(), m_Clients[id]->GetSocket().Get());
			//TestSendFile("E:\\802067.jpg", m_Clients[id].get());
			Logger.AddLog("[Info] User %s login success with socket %d.\n", username, sk);
		}
		break;
	}
	case CMD_REG_GROUP:
	{
		char grname[MAX_USERNAME_LEN]; int grlen;
		char username[MAX_USERNAME_LEN]; int unlen;

		grlen = bf.ReadInt();
		memcpy(grname, bf.ReadChar(), grlen);
		bf.IncPos(grlen);

		unlen = bf.ReadInt();
		memcpy(username, bf.ReadChar(), unlen);
		bf.IncPos(unlen);

		int s = 0;
		if (FindGroup(grname) == -1)
		{
			int IDClient = FindClient(username);
			if (IDClient != -1)
			{
				m_Groups.push_back(std::make_unique<GroupChatSV>(grname, m_Clients[IDClient].get()));
				s = 1;
			}
		}

		// send status success
		Buffer stbf(MAX_BUFFER_LEN);
		stbf.WriteChar(CMD_STATUS);
		stbf.WriteChar(REG_GROUP);
		stbf.WriteChar((char)s);
		if (s)
		{
			stbf.WriteInt(grlen);
			stbf.WriteChar(grname, grlen);
			Logger.AddLog("[Info] %s Register group success.", grname);
		}
		
		SendClient(sk, stbf.Get(), stbf.GetPos());

		break;

		
	}
	case CMD_SEND_USER:
	{
		Client* cl = m_Clients[FindClient(sk)].get();
		char username[MAX_USERNAME_LEN]; int unlen;
		
		// Read username 
		unlen = bf.ReadInt();
		memcpy(username, bf.ReadChar(), unlen);
		bf.IncPos(unlen);

		int TargetID = FindClient(username);
		if (TargetID == -1)
		{
			Buffer stbf(MAX_STATUS_LEN);
			stbf.WriteChar(CMD_STATUS);
			stbf.WriteChar(SEND_TEXT);
			stbf.WriteChar(0);
			stbf.WriteInt(23);
			stbf.WriteChar("Can't found username.", 23);
			SendClient(cl, stbf.Get(), stbf.GetPos());
			Logger.AddLog("[Error] Socket: %d Can't found username: %s", sk, username);
		}
		else
		{
			Client* Target = m_Clients[TargetID].get();
			if (Target->GetStatus() == Client::OFFLINE)
			{
				Buffer stbf(MAX_STATUS_LEN);
				stbf.WriteChar(CMD_STATUS);
				stbf.WriteChar(SEND_TEXT);
				stbf.WriteChar(0);
				stbf.WriteInt(18);
				stbf.WriteChar("User not online.", 18);
				SendClient(cl, stbf.Get(), stbf.GetPos());
				Logger.AddLog("[Error] Socket: %d User not online.", sk);
			}
			else if(Target->GetStatus() == Client::ONLINE)
			{
				char messenger[MAX_BUFFER_LEN]; int textlen;
				textlen = bf.ReadInt();
				memcpy(messenger, bf.ReadChar(), textlen);

				Buffer stbf(MAX_BUFFER_LEN);
				stbf.WriteChar(CMD_SEND_USER);
				stbf.WriteInt(cl->GetUsername().size()+1);
				stbf.WriteChar(cl->GetUsername().c_str(), cl->GetUsername().size() + 1);
				stbf.WriteInt(textlen);
				stbf.WriteChar(messenger, textlen);
				SendClient(Target, stbf.Get(), stbf.GetPos());
				Logger.AddLog("[Info] Socket: %d messenger %s send.", sk, messenger);
			}
		}
		break;
	}
	case CMD_SEND_GROUP:
	{
		char grname[MAX_USERNAME_LEN]; int grlen;
		char msg[MAX_BUFFER_LEN]; int msglen;
		char username[MAX_USERNAME_LEN]; int unlen;

		grlen = bf.ReadInt();
		memcpy(grname, bf.ReadChar(), grlen);
		bf.IncPos(grlen);

		unlen = bf.ReadInt();
		memcpy(username, bf.ReadChar(), unlen);
		bf.IncPos(unlen);

		msglen = bf.ReadInt();
		memcpy(msg, bf.ReadChar(), msglen);
		bf.IncPos(msglen);

		int ID = FindGroup(grname);
		if (ID != -1)
		{
			m_Groups[ID]->SendMsg(username,msg);
			Logger.AddLog("[Info] Msg form %s to %s\n", username, grname);
		}
		break;
	}
	case CMD_SEND_FILE:
	{
		char filename[MAX_BUFFER_LEN];int fnlen;
		char name[MAX_USERNAME_LEN]; int len;
		char username[MAX_USERNAME_LEN]; int unlen;
		m_File.type = *bf.ReadChar();
		bf.IncPos(1);
		len = bf.ReadInt();
		memcpy(name, bf.ReadChar(), len);
		bf.IncPos(len);

		// Read username 
		unlen = bf.ReadInt();
		memcpy(username, bf.ReadChar(), unlen);
		bf.IncPos(unlen);

		fnlen = bf.ReadInt();
		memcpy(filename, bf.ReadChar(), fnlen);
		bf.IncPos(fnlen);

		long abb = 0;
		memcpy(&abb, bf.ReadChar(), sizeof(long));
		bf.IncPos(sizeof(long));
		assert(abb > 0);
		m_File.FileName = filename;
		m_File.size = abb;
		m_File.current = 0;
		m_File.pData = new char[abb];
		m_File.name = name;
		m_File.From = username;
		Logger.AddLog("[File] %s | %d\n", filename, abb);
		FileSending = 0;

		break;
	}
	case CMD_ADD_USER:
	{
		char grname[MAX_USERNAME_LEN]; int grlen;
		char username[MAX_BUFFER_LEN]; int unlen;

		unlen = bf.ReadInt();
		memcpy(username, bf.ReadChar(), unlen);
		bf.IncPos(unlen);

		grlen = bf.ReadInt();
		memcpy(grname, bf.ReadChar(), grlen);
		bf.IncPos(grlen);

		int IDClient = FindClient(username);

		int s = 0;
		if (IDClient != -1)
		{
			int IDGroup = FindGroup(grname);
			if (IDGroup != -1)
			{
				m_Groups[IDGroup]->AddUser(m_Clients[IDClient].get());
				s = 1;
			}
		}

		Buffer stbf(MAX_BUFFER_LEN);
		stbf.WriteChar(CMD_STATUS);
		stbf.WriteChar(ADD_USER);
		stbf.WriteChar((char)s);
		stbf.WriteInt(grlen);
		stbf.WriteChar(grname, grlen);

		SendClient(sk, stbf.Get(), stbf.GetPos());

		if (s) Logger.AddLog("[Info] Add user %s to group %s\n", username, grname);
		break;
	}

	case CMD_GET_GRINFO:
	{
		char grname[MAX_USERNAME_LEN]; int grlen;
		char username[MAX_USERNAME_LEN]; int unlen;

		grlen = bf.ReadInt();
		memcpy(grname, bf.ReadChar(), grlen);
		bf.IncPos(grlen);

		unlen = bf.ReadInt();
		memcpy(username, bf.ReadChar(), unlen);
		bf.IncPos(unlen);

		int IDGroup = FindGroup(grname);
		int IDClient = FindClient(username);
		int num = 0;
		if (IDGroup != -1 && IDClient !=-1)
		{
			num = m_Groups[IDGroup]->GetUserInfo().size();
		}

		Buffer rqbf(MAX_BUFFER_LEN);
		rqbf.WriteChar(CMD_GET_GRINFO);
		rqbf.WriteInt(grlen);
		rqbf.WriteChar(grname, grlen);

		rqbf.WriteInt(num);
		for (int i = 0; i < num; i++)
		{
			const UserInfo& ui = m_Groups[IDGroup]->GetUserInfo()[i];
			if (ui.Name == username) continue;
			rqbf.WriteInt(ui.Name.size()+1);
			rqbf.WriteChar(ui.Name.c_str(), ui.Name.size() + 1);
		}

		SendClient(sk, rqbf.Get(), rqbf.GetPos());
		
		break;
	}
	default:
		Logger.AddLog("[Error] Unknow command.\n ");
		break;
	}
}

void Server::SetAsynch(HWND hwnd)
{
	//WSAAsyncSelect(m_SVSocket.Get(), hwnd, WM_SOCKET, FD_ACCEPT | FD_READ | FD_CLOSE);
}

void Server::TestSendFile(std::string filepath, Client *cl)
{
	FILE* pFile = fopen(filepath.c_str(), "rb");
	if (!pFile) return;
	fseek(pFile, 0, SEEK_END);
	auto size = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	char* data = new char[size];
	fread(data, size, 1, pFile);
	fclose(pFile);

	int pos = filepath.find_last_of("\\");
	std::string filename = filepath.substr(pos + 1, filepath.size() - pos);
	printf("FIlename: %s\n", filename.c_str());

	Buffer stbf(MAX_BUFFER_LEN);
	stbf.WriteChar(CMD_SEND_FILE);
	stbf.WriteInt(filename.size() + 1);
	stbf.WriteChar(filename.c_str(), filename.size() + 1);
	stbf.WriteChar((char*)&size, sizeof(long));

	cl->GetSocket().Send(stbf.Get(), stbf.GetPos());

	long len = size;
	char* data2 = data;

	m_File.pData = data;
	m_File.size = size;
	while (len > 0)
	{
		
		long amount = cl->GetSocket().Send(data2, len);
		//int result = WSASend(cl->GetSocket().Get(), &DataBuf, 1, &RecvBytes, 0, 0, 0);
		Logger.AddLog("Sending...%ld\n",amount);
		if (amount == SOCKET_ERROR)
		{
			Logger.AddLog("[File] Send file error %d.\n", WSAGetLastError());
			break;
		}
		else if (amount == 0)
		{
			Logger.AddLog("[File]0 byte send.\n", WSAGetLastError());
			break;
		}
		else
		{
			data2 += amount;
			len -= amount;
		}
	
	}
}

void Server::Send2ND(Client *cl)
{
	long len = m_File.size;
	char* data2 = m_File.pData;

	while (len > 0)
	{
		Logger.AddLog("Sending...\n");
		long amount = cl->GetSocket().Send(data2, len);
		//int result = WSASend(cl->GetSocket().Get(), &DataBuf, 1, &RecvBytes, 0, 0, 0);
		if (amount == SOCKET_ERROR)
		{
			Logger.AddLog("[File] Send file error %d.\n", WSAGetLastError());
			break;
		}
		else if (amount == 0)
		{
			Logger.AddLog("[File] Send file error %d.\n", WSAGetLastError());
		}
		else
		{
			data2 += amount;
			len -= amount;
		}

	}
}

void Server::LoadUser()
{
	std::ifstream fp("Data/data.bin");
	if (!fp)
	{
		std::cout << "Error: Can't load user data.\n";
		return;
	}
	std::string un, pw;
	while (fp >> un >> pw)
	{
		m_Clients.push_back(std::make_unique<Client>(INVALID_SOCKET, un.c_str(), pw.c_str()));
	}

	fp.close();
}

void Server::WriteUser()
{
	std::ofstream fp("Data/data.bin");
	if (!fp)
	{
		std::cout << "Error: Can't open user data.\n";
		return;
	}

	for(size_t i=0; i<m_Clients.size(); i++)
	{
		
		fp << m_Clients[i]->GetUsername() <<" "<< m_Clients[i]->GetPassWord() << std::endl;

	}

	fp.close();
}

bool Server::CheckUser(const char * username)
{
	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i]->GetUsername() == username) return true;
	}

	return false;
}

int Server::FindClient(SOCKET sk)
{
	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i]->GetSocket().Get() == sk) return i;
	}

	return -1;
}

int Server::FindClient(const char * username, const char* password)
{
	for (size_t i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i]->GetUsername() == username)
		{
			if (password)
			{
				if (m_Clients[i]->GetPassWord() == password) return i;
				else return -1;
			}
			else return i;
		}
	}

	return -1;
}

int Server::FindGroup(const char * name)
{
	for (size_t i = 0; i < m_Groups.size(); i++)
	{
		if (m_Groups[i]->GetName() == name) return i;
	}
	return -1;
}

bool Server::SendClient(Client*cl, const char * buffer, int len)
{
	return SendClient(cl->GetSocket().Get(), buffer,len);
}

bool Server::SendClient(SOCKET sk, const char * buffer, int len)
{
	int iResult = send(sk, buffer, len, 0);

	if (iResult == SOCKET_ERROR) {
		Logger.AddLog("[Error] Send failed with error: %d\n", WSAGetLastError());
		return 0;
	}

	return 1;
}

bool Server::SendFile(Client * cl)
{
	Buffer stbf(MAX_BUFFER_LEN);
	stbf.WriteChar(CMD_SEND_FILE);
	stbf.WriteInt(m_File.FileName.size() + 1);
	stbf.WriteChar(m_File.FileName.c_str(), m_File.FileName.size() + 1);
	stbf.WriteChar((char*)&m_File.size, sizeof(long));

	cl->GetSocket().Send(stbf.Get(), stbf.GetPos());

	long len = m_File.size;
	char* data2 = m_File.pData;
	while (len > 0)
	{
		
		long amount = cl->GetSocket().Send(data2, len);
		Logger.AddLog("Sending byte %d\n", (int)amount);
		if (amount == SOCKET_ERROR)
		{
			Logger.AddLog("[File] Send file error.\n");
			break;
		}
		else if (amount == 0)
		{
			Logger.AddLog("[File] 0 byte send.\n");
			break;
		}
		else
		{
			data2 += amount;
			len -= amount;
		}
	}

	return 1;
}

