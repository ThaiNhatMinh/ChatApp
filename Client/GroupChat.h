#pragma once


class GroupChat : public ChatDiaglog
{
private:
	int status;
public:
	GroupChat(const char* name, Client* cl, CLSocket& sk) :ChatDiaglog(name,cl,sk), status(-1){};
	~GroupChat() = default;


	virtual void	Draw(); 
	void			Status(int s);
};