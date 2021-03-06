// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <map>

#include <wchar.h>
#include <cassert>
#include <iomanip>
#include <set>
#include <iterator>
#include <algorithm>
#include <list>
#include <strstream>
#include <functional>
#include <thread>

#include "..\Shared\header.h"

// server only
#include "GroupChatSV.h"
#include "Client.h"
#include "SVSocket.h"
#include "Server.h"
#include "ServerApp.h"
// Need to link with Ws2_32.lib
// #pragma comment (lib, "Mswsock.lib")
