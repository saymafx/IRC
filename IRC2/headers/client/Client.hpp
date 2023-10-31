#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include <map>
# include <sstream>
# include <vector>
# include <set>
# include "../socket/Socket.hpp"
# include "../channel/Channel.hpp"
#include <ctime>


class Client : public Socket
{
public:
	typedef enum e_status{
		connected,
		idle,
		busy,
		disconnected
	}status;

	typedef enum e_rights{
		operators,
		regular_user,
	}rights;
	
private:

	Client::status										_status;
	Client::rights										_rights;
	std::map<std::string, std::vector<std::string> >	_request;
	std::vector<std::string> 							_myChannels;
	std::vector<std::string>							_myMessage;
	std::set<int>										_receivers;		

	std::map<int, std::string>							_errorCode;
	std::string											_nick;

public:
	Client(const int sockclt);
	//Client(const Client &tocopy) {*this = tocopy;};
	// Client &operator=(const Client &toassign) {
	
	// _status = toassign._status;
	// _rights = toassign._rights;
	// _request = toassign._request;
	// _myChannels = toassign._myChannels;
	// _myMessage = toassign._myMessage;
	// _receivers = toassign._receivers;		
	// _errorCode = toassign._errorCode;
	// return *this;
	// }
	~Client();

	void setStatus(const Client::status & status);
	void setRights(const Client::rights & rights);
	void setMessage(std::string  message);
	std::map<std::string, std::vector<std::string> >	&setRequest(void);
	void  setReceivers(int socket);


	void clearMessage(void);

	const std::vector<std::string> &getMessage() const;
	Client::status getStatus() const;
	Client::rights getRights() const;
	const std::set<int> & getReceivers(void) const;
	const std::map<std::string, std::vector<std::string> >	&getRequest(void) const;
	const std::string & getName(void) const;
	const std::string & getUserName(void) const;

	void  addNewChannel(std::string channelname);

	clock_t getLastJoinTime() const;
    void setLastJoinTime(clock_t time);

	std::string		getNick() const;
	void			setNick(std::string);


};

#endif
