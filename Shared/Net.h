#pragma once

#define WM_SOCKET WM_USER+1



#define DEFAULT_PORT "27015"
#define MAX_USERNAME_LEN 64
#define MAX_PASSWORD_LEN 64
#define MAX_STATUS_LEN 128
#define MAX_BUFFER_LEN 1024

enum Command
{
	/* format messenger
		char:		cmd code
		int:		username length
		char[]:		username
		int:		password length
		char[]:		password
	*/
	CMD_REG_USER,		// command register user

	/* format messenger
		char:		cmd code
		int:		username length
		char[]:		username
		int:		password length
		char[]:		password
	*/
	CMD_LOGIN_USER,		// command login user

	/* format messenger
		char:		cmd code
	
	*/
	CMD_REG_GROUP,		// command register group chat

	/* command to send a messenger to user
		Format messenger
		char:		cmd code
		int:		username length (to/from)
		char[]:		username (to/from)
		int:		messenge length
		char[]:		text message
	*/
	CMD_SEND_USER,		

	CMD_SEND_GROUP,		// command to send a messenger to group
	CMD_SEND_FILES,		// command to send a file to user

	CMD_RECV,			// command to get a messenger

	/* format messenger
		char:		cmd code
		char:		status code
		char:		status 0/1 (success/fail)
		int:		messenge length
		char[]:		text message
	*/
	CMD_STATUS,			// send status

	CMD_COUNT			// num command
};

enum Status
{
	LOGIN_USER,
	SEND_TEXT,
	SEND_FILE,
	REG_USER,
	REG_GROUP
};