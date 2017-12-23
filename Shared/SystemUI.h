#pragma once
#include "header.h"
#include "imgui\imgui.h"

class Shader;
class imguiMesh;
class SystemUI
{

private:
	GLuint m_FontTexture;
	std::unique_ptr<Shader> m_pShader;
	std::unique_ptr<imguiMesh> m_Mesh;
	GLFWwindow* w;
	bool m_MousePress[3];
	float m_MouseWhell;
	float m_Time;
	ImFont* m_Font;
	ImFont* m_SysbolFont;
protected:
	void CreateFontsTexture();
	
public:
	SystemUI(Windows* w);
	~SystemUI();

	void OnRenderDrawLists(ImDrawData* data);
	void NewFrame();
	ImFont* GetCurrentFont() {
		return m_Font;
	};
	ImFont* GetSysbolFont()
	{
		return m_SysbolFont;
	}
	friend void ImGui_ImplGlfwGL3_MouseButtonCallback(GLFWwindow* w, int button, int action, int /*mods*/);
	friend void ImGui_ImplGlfwGL3_ScrollCallback(GLFWwindow* w, double /*xoffset*/, double yoffset);
};
