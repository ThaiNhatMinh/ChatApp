#pragma once
//#include "imgui\imgui.h"
#include "CLSocket.h"
#include "Client.h"
struct Messenger
{
	ImVec4 color;
	std::string Text;
	std::string Name;
};

class ChatDiaglog
{
private:
	std::vector<Messenger>    m_ChatData;
	bool						ScrollToBottom;
	bool						m_Open;
	std::string					m_Title;
	char						InputBuf[256];
	std::vector<std::string>    m_UserList;
	Client*						m_Client;
	CLSocket&					m_Socket;
public:
	ChatDiaglog(const char* name, Client* cl,CLSocket& sk) :m_Title(name), m_Open(1), m_Client(cl), m_Socket(sk){ m_UserList.push_back(name); };
	~ChatDiaglog() = default;
	void    Clear() { m_ChatData.clear(); }

	std::string& GetName() { return m_Title; }
	bool IsOpen() {
		return m_Open;
	}
	void Open(bool a) { m_Open = a; }
	void    AddMessenger(const char* user,const char* text)
	{
		Messenger mg;
		mg.Name = user;
		mg.Text = text;
		if(m_Title==user) mg.color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
		else mg.color = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
		m_ChatData.push_back(mg);
	}

	void    Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin(m_Title.c_str(), &m_Open);
		
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

		for (size_t i = 0; i < m_ChatData.size(); i++)
		{
			ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, col);
			ImGui::Text("[%s] %s",m_ChatData[i].Name.c_str(), m_ChatData[i].Text.c_str());
			ImGui::PopStyleColor();
		}

		if (ScrollToBottom)
			ImGui::SetScrollHere();
		ScrollToBottom = false;
		ImGui::PopStyleVar();
		ImGui::EndChild();


		if (ImGui::InputText("Input", InputBuf, 256,ImGuiInputTextFlags_EnterReturnsTrue))
		{
			for (auto& el : m_UserList)
			{
				Buffer bf(MAX_BUFFER_LEN);
				bf.WriteChar(CMD_SEND_USER);
				bf.WriteInt(el.size() + 1);
				bf.WriteChar(el.c_str(), el.size() + 1);
				bf.WriteInt(strlen(InputBuf) + 1);
				bf.WriteChar(InputBuf, strlen(InputBuf) + 1);
				
				m_Socket.Send(bf.Get(), bf.GetPos());

			}

			AddMessenger(m_Client->GetUsername().c_str(), InputBuf);
			strcpy(InputBuf, "");
		}
		ImGui::End();
	}
};