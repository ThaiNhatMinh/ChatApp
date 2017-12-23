#pragma once


class Buffer
{
private:
	int pos;
	int len;
	char* m_pPointer;
public:
	Buffer(char* data,int len);
	~Buffer() = default;

	void			IncPos(int offset);
	void			DecPos(int offset);

	char*			ReadChar();
	int				ReadInt();
	float			ReadFloat();

	bool			WriteChar(const char* p,int size);
	bool			WriteInt(int p);
	bool			WriteFloat(float p);

	char*			Get();
	int				GetPos();
};