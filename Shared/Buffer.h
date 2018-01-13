#pragma once


class Buffer
{
private:
	int pos;
	int len;
	char* m_pPointer;
	bool canDelete;
public:
	Buffer(int len = MAX_BUFFER_LEN);
	Buffer(char* p, int len);
	~Buffer();

	void			IncPos(int offset);
	void			DecPos(int offset);

	char*			ReadChar();
	int				ReadInt();
	float			ReadFloat();

	bool			WriteChar(const char* p,int size);
	bool			WriteChar(char c);
	bool			WriteInt(int p);
	bool			WriteFloat(float p);

	char*			Get();
	int				GetPos();
};