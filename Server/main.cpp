#include "stdafx.h"
#include "Server.h"

int __cdecl main(void)
{
	Server ma;
	if (!ma.Init())
	{
		printf("Cannot init server.");
		return 0;
	}

	ma.Loop();

	return 0;
}

