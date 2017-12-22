#pragma once

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

enum Command
{
	CMD_REG,
	CMD_SEND,
	CMD_RECV,
	CMD_COUNT
};