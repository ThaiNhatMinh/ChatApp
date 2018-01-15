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
	ScrollToBottom = true;
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
	if (!m_Open) return;
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

	ImGui::End();
}
