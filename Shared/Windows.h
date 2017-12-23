#pragma once
#include "header.h"




class Windows
{
private:
	GLuint m_iWidth;
	GLuint m_iHeight;
	vec2 m_Pos;
	GLFWmonitor* m_pMonitor;
	GLFWwindow* m_pWindow;
	std::string m_WindowTitle;
	GLboolean m_bIsFullscreen;
	GLuint m_iScreenWidth;
	GLuint m_iScreenHeight;

private:
	// Read config from XMLElement
	// return init pos window
	vec2 ReadConfig();
public:
	Windows();
	~Windows();
	void SetSize(int W, int H);
	void SetPos(vec2 pos);
	void ShowWindows();
	void HideWindows();
	void EnableFullScreen(bool enable);
	void SwitchMode();
	int ShouldClose();
	void Close();
	void SetMouse(bool t);
	vec2 GetWindowSize(){ return vec2(m_iWidth, m_iHeight); };
	GLFWwindow* Window() {		return m_pWindow;	};
	HWND GetHandle() { return glfwGetWin32Window(m_pWindow); }
	void PrintMsg(const char* msg) {
		printf("%s\n", msg);
	};
};

