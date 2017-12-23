#pragma once

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

enum Command
{
	CMD_REG_USER,
	CMD_REG_GROUP,

	CMD_SEND_USER,
	CMD_SEND_GROUP,
	CMD_SEND_FILES,

	CMD_RECV,
	CMD_COUNT
};