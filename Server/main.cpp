#include "stdafx.h"
#include "Server.h"

int WinMain(
	 HINSTANCE hInstance,
	 HINSTANCE hPrevInstance,
	 LPSTR     lpCmdLine,
	 int       nCmdShow
)
{

	ServerApp app;
	app.RunMainLoop();
	return 0;
}

