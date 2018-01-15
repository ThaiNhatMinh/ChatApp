#include "stdafx.h"

void GroupChat::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
	ImGui::Begin(m_Title.c_str(), &m_Open, ImGuiWindowFlags_NoResize);

	// ScrollingRegion
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("ScrollingRegion", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.7f, 220), true, ImGuiWindowFlags_HorizontalScrollbar);
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
	}
	ImGui::SameLine();

	// Add user button
	{
		static char un[MAX_USERNAME_LEN];

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("UserInfoRegion", ImVec2(0, 220), true);

		// print user list

		for (size_t i = 0; i < m_UserList.size(); i++)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, m_UserList[i].Color);
			ImGui::Text("%d. %s", i, m_UserList[i].Name.c_str());
			ImGui::PopStyleColor();
		}

		if (ImGui::Button("Add user")) ImGui::OpenPopup("AddUser");
		if (ImGui::BeginPopupModal("AddUser", nullptr, ImGuiWindowFlags_NoResize))
		{

			if (ImGui::InputText("UserName", un, MAX_USERNAME_LEN, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				// Request add user to group
				Buffer bf(MAX_BUFFER_LEN);
				bf.WriteChar(CMD_ADD_USER);
				bf.WriteInt(strlen(un) + 1);
				bf.WriteChar(un, strlen(un) + 1);
				bf.WriteInt(m_Title.size() + 1);
				bf.WriteChar(m_Title.c_str(), m_Title.size() + 1);

				m_Socket.Send(bf.Get(), bf.GetPos());

				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("OK", ImVec2(120, 0)) && strlen(un) > 0)
			{
				// Request add user to group
				Buffer bf(MAX_BUFFER_LEN);
				bf.WriteChar(CMD_ADD_USER);
				bf.WriteInt(strlen(un) + 1);
				bf.WriteChar(un, strlen(un) + 1);
				bf.WriteInt(m_Title.size() + 1);
				bf.WriteChar(m_Title.c_str(), m_Title.size() + 1);

				m_Socket.Send(bf.Get(), bf.GetPos());

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		if (status == 0)
		{
			status = -1;
			ImGui::OpenPopup("AddUserF");
		}
		else if (status == 1)
		{
			status = -1;
			AddUser(un);

		}
		if (ImGui::BeginPopupModal("AddUserF", nullptr, ImGuiWindowFlags_NoResize))
		{
			ImGui::Text("Add user fail!");
			if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}




	// Add messenger
	{
		if (ImGui::InputText("Input", InputBuf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			Buffer bf(MAX_BUFFER_LEN);
			bf.WriteChar(CMD_SEND_GROUP);
			bf.WriteInt(m_Title.size() + 1);
			bf.WriteChar(m_Title.c_str(), m_Title.size() + 1);
			bf.WriteInt(m_Client->GetUsername().size() + 1);
			bf.WriteChar(m_Client->GetUsername().c_str(), m_Client->GetUsername().size() + 1);
			bf.WriteInt(strlen(InputBuf) + 1);
			bf.WriteChar(InputBuf, strlen(InputBuf) + 1);

			m_Socket.Send(bf.Get(), bf.GetPos());

			// Don't need this, server will send to all member
			//AddMessenger(m_Client->GetUsername().c_str(), InputBuf);
			strcpy(InputBuf, "");
		}
		// Demonstrate keeping auto focus on the input box
		if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
			ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
	}


	ImGui::End();
}

void GroupChat::Status(int s)
{
	status = s;
}
