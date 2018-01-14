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

	/*	Command register group chat
		Format messenger
		char:		cmd code
		int:		group name length
		char[]:		group name
		int:		admin name length
		char[]:		admin name
	*/
	CMD_REG_GROUP,		

	/* command to send a messenger to user
		Format messenger
		char:		cmd code
		int:		username length (to/from)
		char[]:		username (to/from)
		int:		messenge length
		char[]:		text message
	*/
	CMD_SEND_USER,

	/* command to add a user to a group
		Format messenger
		char:		cmd code
		int:		username length (to/from)
		char[]:		username (to/from)
		int:		group name length
		char[]:		text name
	*/
	CMD_ADD_USER,

	/* command to send a messenger to group
		Format messenger
		char:		cmd code
		int:		group name length (to/from)
		char[]:		group name (to/from)
		int:		username length (to/from)
		char[]:		username (to/from)
		int:		messenge length
		char[]:		text message
	*/
	CMD_SEND_GROUP,

	/* command send to get group info
		Format messenger
		char:		cmd code
		int:		group name length (to/from)
		char[]:		group name (to/from)
		int:		user name length (to/from)
		char[]:		user name (to/from)
		// Result format. Not include username request
		int:		group name length (to/from)
		char[]:		group name (to/from)
		int:		num user
		int:		user name length (to/from)
		char[]:		user name (to/from)
	*/
	CMD_GET_GRINFO,

	CMD_SEND_FILES,		// command to send a file to user

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
	REG_GROUP,
	ADD_USER
};