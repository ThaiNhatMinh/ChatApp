#pragma once

class Server;

class ThreadRAII
{
private:
	std::thread m_thread;
public:
	template<class ...Args>
	ThreadRAII(std::function<void(void* sv)>, Args &&...args);
	~ThreadRAII();
};

template<class ...Args>
ThreadRAII::ThreadRAII(std::function<void(void* sv)> f, Args &&...args)
{
	m_thread = std::thread(f, std::forward<Args>(args)...);
}
