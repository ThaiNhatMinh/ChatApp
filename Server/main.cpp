#include "stdafx.h"
#include "Server.h"

int __cdecl main(void)
{
	//Server ma;
	//if (!ma.Init())
	//{
	//	printf("Cannot init server.");
	//	return 0;
	//}

	//ma.Loop();


	char test[DEFAULT_BUFLEN];
	Buffer bff(test, DEFAULT_BUFLEN);
	bff.WriteChar("TEST", 5);
	bff.WriteInt(5);
	bff.WriteFloat(1.9f);

	Buffer w(test, DEFAULT_BUFLEN);

	char ttt[220];
	memcpy(ttt, w.ReadChar(), 5);
	w.IncPos(5);
	int a = w.ReadInt();
	float c = w.ReadFloat();
	//std::cout << "Result: " << ttt  << " " <<a  << " " <<c ;
	
	ServerApp app;
	app.RunMainLoop();
	return 0;
}

