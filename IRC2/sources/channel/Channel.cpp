# include "../../headers/channel/Channel.hpp"
# include "../../headers/server/Server.hpp"

	Channel::Channel(const std::string & channelname, const std::string password) 
		: _channelName(channelname), _nbrClients(0), /*_rights(STANDARD),*/
		_channelPassword(password), invite_only(UNSET), topic_restriction(SET),  _connectedUsers(), _userList(), _adminList(),  
		_topic("Mon topic est les poulets ont flambes."), _invitedUsers(){
	}

	Channel::~Channel(void){}

	//void Channel::setRights(const Channel::ch_mod & rights){ _rights = rights; }
	std::set<std::string> & Channel::setConnected(void){ return(_connectedUsers);}
	std::map<std::string, Channel::ch_rights> & Channel::setList(void){ return(_userList);}
	void Channel::setNbr(size_t nbrClients){ _nbrClients = nbrClients;}
	void Channel::setTopic(std::string topic) { _topic = topic; }
	void Channel::setInvite(std::string user) { _invitedUsers.insert(user);}
	bool Channel::inviteStatus(std::string username) {
		for (std::set<std::string>::iterator invit = _invitedUsers.begin(); invit != _invitedUsers.end(); ++invit)
		{
			if(username == *invit)
				return(true);
		}
		return (false);
	}
	void Channel::setChannelPass(std::string channelPass){ _channelPassword = channelPass;};
	void Channel::addConnectedUser(std::string username){ _connectedUsers.insert(username);} 
	const std::string & Channel::getName(void) const { return(_channelName);}
	const std::string & Channel::getPassword(void) const { return(_channelPassword);}
	std::string Channel::getTopic(void) const { return (_topic);}
	const std::set<std::string> & Channel::getConnected(void) const{ return(_connectedUsers);}
	const std::set<std::string> & Channel::getInvitedUsers(void) const { return _invitedUsers;}
	const std::map<std::string, Channel::ch_rights> & Channel::getList(void) const{ return(_userList);}
	//const Channel::ch_mod & Channel::getRights(void) { return (_rights);}
	void Channel::setChanAdmin(std::string username){ (_adminList.push_back(username));}
    const std::list<std::string> & Channel::getChanAdmin(void) const{ return(_adminList);}
    const std::string & Channel::getChannelPass(void) const{ return (_channelPassword);}
    int	 Channel::isOperator(std::string User) const
	{
		// parcourir la liste des admins et vérifier si User en fait partie.
		for (std::list<std::string>::const_iterator itAdmin = this->_adminList.begin(); itAdmin != this->_adminList.end(); ++itAdmin)
		{
			if (User == *itAdmin)
				return (OPERATOR);
		}
		return (REGULAR);
	} // fonction qui determine si un client (User ) est opérateur dans un channel donné (this).
	
	size_t Channel::getNbr(void) const{ return(_nbrClients);}

	int	Channel::setMode(int mode, int action, int limit, std::string arg, std::string chan, std::string cl) // actio SET/UNSET
	{
		(void)arg;
		(void)limit;
		(void)action;
		(void)chan;
		(void)cl;
		int i = 0;
		if (mode == INVITE_ONLY_MODE)
		{
			if (this->invite_only != action)
				i = 1;
			this->invite_only = action;
		}
		else if (mode == TOPIC_RESTRICT_MODE)
		{
			if (this->topic_restriction != action)
				i = 1;
			this->topic_restriction = action;
		}
		else if (mode == PASSWORD)
		{
			if (this->_channelPassword != arg)
				i = 1;
			if (action == UNSET)
				this->_channelPassword = "";
			else
				this->_channelPassword = arg;
		}
		else if (mode == LIMIT)
		{
			if (static_cast<int>(this->_nbrClients) != limit)
				i = 1;
			if (action == UNSET)
				this->_nbrClients = 0;
			if (action == SET || limit > 0 || limit >=  static_cast<int>(this->getConnectedUserCount()))
				this->_nbrClients = limit;
		}
		else if (mode == PRIVILEGE)
		{
			if (action == SET && this->isOperator(arg) == REGULAR)
			{
				this->setChanAdmin(arg);
				i = 1;
			}
			else if (action == UNSET && this->isOperator(arg) == OPERATOR)
			{
				_adminList.remove(arg); // This will remove all occurrences of arg from the list
				i = 1;
			}
		}
		return i;
	}

	int 	Channel::getMode(int mode, std::string arg) // actio SET/UNSET
	{
		(void)arg;
		if (mode == INVITE_ONLY_MODE)
			return (this->invite_only);
		if (mode == LIMIT) {
			if (this->_nbrClients > 0)
				return SET;
			return UNSET;
		}
		if (mode == TOPIC_RESTRICT_MODE)
			return (this->topic_restriction);
		if (mode == PASSWORD) {
			if (this->getPassword().empty()) {
				// std::cout << "NO PASSWORD IS SET" << std::endl;
				return UNSET;
			}
			else
				return SET;
		}
		return (-1);
	}

	bool	Channel::isConnected(std::string user) const{
		for (std::set<std::string>::const_iterator itCo = _connectedUsers.begin(); itCo != _connectedUsers.end(); ++itCo)
		{
			if (user == *itCo)
				return (true);
		}
		return (false);
	}

	size_t 	Channel::getConnectedUserCount() const {
		return _connectedUsers.size();
	}
