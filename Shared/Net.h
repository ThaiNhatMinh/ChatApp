#pragma once

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define MAX_USERNAME_LEN 64
#define MAX_PASSWORD_LEN 64

enum Command
{
	CMD_REG_USER,		// command register user
	CMD_REG_GROUP,		// command register group chat

	CMD_SEND_USER,		// command to send a messenger to user
	CMD_SEND_GROUP,		// command to send a messenger to group
	CMD_SEND_FILES,		// command to send a file to user

	CMD_RECV,			// command to get a messenger

	CMD_REG_SUCCESS,	// register success
	CMD_REG_FAIL,		// register fail
	CMD_SEND_USER_SUCCESS,
	CMD_SEND_USER_FAIL,
	CMD_COUNT			// num command
};