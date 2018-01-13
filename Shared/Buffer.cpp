#include "stdafx.h"
#include "Buffer.h"
#include <string.h>


Buffer::Buffer(int len) :m_pPointer(new char[len]), pos(0), len(len), canDelete(1)
{
}

Buffer::Buffer(char * p, int len):m_pPointer(p),len(len), canDelete(0)
{
}

Buffer::~Buffer()
{
	if(canDelete) delete[] m_pPointer;
}

void Buffer::IncPos(int offset)
{
	pos += offset;
}

void Buffer::DecPos(int offset)
{
	pos -= offset;
}

char* Buffer::ReadChar()
{
	return &m_pPointer[pos];
}

int Buffer::ReadInt()
{
	if (pos + sizeof(int) >= len) return 0;
	int t;
	// copy 4 byte from data to int;
	memcpy(&t, &m_pPointer[pos], sizeof(int));
	pos += sizeof(int);
	return t;
}

float Buffer::ReadFloat()
{
	if (pos + sizeof(float) >= len) return 0;
	float t;
	// copy 4 byte from data to float;
	memcpy(&t, &m_pPointer[pos], sizeof(float));
	pos += sizeof(float);
	return t;
}

bool Buffer::WriteChar(const char * p, int size)
{
	if (pos >= len) return false;
	memcpy(&m_pPointer[pos], p, size);
	pos += size;
	return true;
}

bool Buffer::WriteChar(char c)
{
	if (pos >= len) return false;
	m_pPointer[pos] = c;
	pos++;
	return true;
}

bool Buffer::WriteInt(int p)
{
	if (pos+ sizeof(int) >= len) return false;

	memcpy(&m_pPointer[pos], &p, sizeof(int));
	pos += sizeof(int);

	
	return true;
}

bool Buffer::WriteFloat(float p)
{
	if (pos + sizeof(float) >= len) return false;

	memcpy(&m_pPointer[pos], &p, sizeof(float));
	pos += sizeof(float);

	return true;
}

char * Buffer::Get()
{
	return m_pPointer;
}

int Buffer::GetPos()
{
	return pos;
}

