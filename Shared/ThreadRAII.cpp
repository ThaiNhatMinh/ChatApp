#include "stdafx.h"
#include "ThreadRAII.h"



ThreadRAII::~ThreadRAII()
{
	// Check if thread is joinable then detach the thread
	if (m_thread.joinable())
	{
		m_thread.detach();
	}
}
