// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
/* srand example */
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


int __cdecl main(int argc, char **argv)
{

	Client cl;
	srand(time(NULL));
	int id = rand() % 1000;
	char buffer[DEFAULT_BUFLEN];
	if (!cl.Init())
	{
		printf("Cannot init client.");
		return 0;
	}
	sprintf_s(buffer, "This is a test message from: %d", id);
	char c;
	while (1)
	{
		scanf_s("%c", &c);

		if(c==' ') cl.Send(buffer);
		else if (c == 'q') break;
	}
	return 1;
}
