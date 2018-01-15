#include "stdafx.h"
#include "ChatDialog.h"


ImVec4 ChatDiaglog::GetColor(const std::string & username)
{
	for (auto& el : m_UserList)
	{
		if (el.Name == username) return el.Color;
	}

	// if not found add user
	AddUser(username);
	return m_UserList.front().Color;
}

void ChatDiaglog::SendFile(char type)
{
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name
	//HANDLE hf;              // file handle

							// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_Hwnd;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{

		//m_CurrentImage[m_iActiveImage]->LoadTexture(ofn.lpstrFile);
		SendFile(ofn.lpstrFile,type);
	}

}


bool ChatDiaglog::SendFile(std::string filepath, char type)
{
	FILE* pFile = fopen(filepath.c_str(), "rb");
	if (!pFile) return false;
	fseek(pFile, 0, SEEK_END);
	auto size = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	char* data = new char[size];
	fread(data, size, 1, pFile);
	fclose(pFile);

	int pos = filepath.find_last_of("\\");
	std::string filename = filepath.substr(pos + 1, filepath.size() - pos);
	printf("FIlename: %s\n", filename.c_str());
	//int buffersize = 1 + 4 + filename.size() + 1 + 4 + size;
	//char* buffer = new char[buffersize];
	Buffer bf(MAX_BUFFER_LEN);
	bf.WriteChar(CMD_SEND_FILE);
	bf.WriteChar(type);
	bf.WriteInt(m_UserList[0].Name.size() + 1);
	bf.WriteChar(m_UserList[0].Name.c_str(), m_UserList[0].Name.size() + 1);

	bf.WriteInt(m_Client->GetUsername().size() + 1);
	bf.WriteChar(m_Client->GetUsername().c_str(), m_Client->GetUsername().size() + 1);

	bf.WriteInt(filename.size() + 1);
	bf.WriteChar(filename.c_str(), filename.size() + 1);
	bf.WriteChar((const char*)&size, sizeof(size));

	m_Socket.Send(bf.Get(), bf.GetPos());


	FileSendInfo* pFI = new FileSendInfo;
	pFI->pData = data;
	pFI->size = size;
	//pFI->pSocket = &m_Socket;

	/*auto SendFileFunc = [](void* pPointer)
	{
		FileSendInfo* pFI = (FileSendInfo*)pPointer;
		char* data = pFI->pData;
		long len = pFI->size;
		printf("Begin send file...\n");
		printf("File size: %d\n", pFI->size);
		while (len > 0)
		{
			printf("Sending...\n");
			long amount = pFI->pSocket->Send(data, len);
			if (amount == SOCKET_ERROR)
			{
				printf("Send file error.\n");
				return;
			}
			else
			{
				printf("Amount: %d\n", amount);
				data += amount;
				len -= amount;
			}
		}
		printf("Finish send file...\n");
		delete[] pFI->pData;
		delete pFI;

	};
	*/
	char* data2 = pFI->pData;
	long len = pFI->size;
	printf("Begin send file...\n");
	printf("File size: %d\n", pFI->size);
	while (len > 0)
	{
		printf("Sending...\n");
		long amount = m_Socket.Send(data2, len);
		if (amount == SOCKET_ERROR)
		{
			printf("Send file error.\n");
			break;
		}
		else
		{
			printf("Amount: %d\n", amount);
			data2 += amount;
			len -= amount;
		}
	}
	printf("Finish send file...\n");
	delete[] pFI->pData;
	delete pFI;
	//m_FileThread.push_back(std::make_unique<ThreadRAII>(SendFileFunc, pFI));

}

void ChatDiaglog::Clear() { m_ChatData.clear(); }

std::string & ChatDiaglog::GetName() { return m_Title; }

bool ChatDiaglog::IsOpen() {
	return m_Open;
}

void ChatDiaglog::Open(bool a) { m_Open = a; }

void ChatDiaglog::AddMessenger(const char * user, const char * text)
{
	Messenger mg;
	mg.Name = user;
	mg.Text = text;
	m_ChatData.push_back(mg);
}

void ChatDiaglog::AddUser(std::string name) {

	for (auto& el : m_UserList)
	{
		if (el.Name == name) return;
	}

	UserInfo ui;
	ui.Name = name;
	srand((time(NULL)));
	ui.Color = ImVec4((rand() % 10 +10) / 20.0f, (rand() % 10 + 10) / 20.0f, (rand() % 10 + 10) / 20.0f, (rand() % 10 + 10) / 20.0f);
	m_UserList.push_back(ui);
}

void ChatDiaglog::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
	ImGui::Begin(m_Title.c_str(), &m_Open);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), true, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

	for (size_t i = 0; i < m_ChatData.size(); i++)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, GetColor(m_ChatData[i].Name));
		ImGui::Text("[%s] %s", m_ChatData[i].Name.c_str(), m_ChatData[i].Text.c_str());
		ImGui::PopStyleColor();
	}

	if (ScrollToBottom)
		ImGui::SetScrollHere();
	ScrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::PopStyleVar();

	if (ImGui::InputText("Input", InputBuf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		for (auto& el : m_UserList)
		{
			Buffer bf(MAX_BUFFER_LEN);
			bf.WriteChar(CMD_SEND_USER);
			bf.WriteInt(el.Name.size() + 1);
			bf.WriteChar(el.Name.c_str(), el.Name.size() + 1);
			bf.WriteInt(strlen(InputBuf) + 1);
			bf.WriteChar(InputBuf, strlen(InputBuf) + 1);

			m_Socket.Send(bf.Get(), bf.GetPos());

		}

		AddMessenger(m_Client->GetUsername().c_str(), InputBuf);
		strcpy(InputBuf, "");
	}
	// Demonstrate keeping auto focus on the input box
	if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

	ImGui::SameLine();

	if (ImGui::Button("Send a file"))
	{
		SendFile(0);
	}

	

	ImGui::End();
}
