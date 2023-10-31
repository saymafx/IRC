# include "../../headers/client/Client.hpp"

	Client::Client(const int sockclt) : Socket(sockclt), _status(disconnected), _rights(regular_user), _request(), _myChannels(), _myMessage(), _receivers() {
		_errorCode.insert(std::make_pair(001, "Welcome"));
		std::cout << "Client connected : " << sockclt << std::endl;
	}

	Client::~Client(){
		std::cout << "Client disconnected : " << _socket << std::endl;
	}

	void Client::setStatus(const Client::status & status){ _status = status;}
	void Client::setRights(const Client::rights & rights){ _rights = rights;}
	std::map<std::string, std::vector<std::string> >	&Client::setRequest(void) {return(_request);}
	void  Client::setMessage(std::string  message) {_myMessage.push_back(message);}
	void  Client::setReceivers(int socket) {_receivers.insert(socket);}
	
	Client::status Client::getStatus(void) const {return(_status);}
	Client::rights Client::getRights(void) const {return(_rights);}
	const std::set<int> & Client::getReceivers(void) const {return(_receivers);}
	const std::map<std::string, std::vector<std::string> >	&Client::getRequest(void) const {return(_request);}
	const std::vector<std::string> &Client::getMessage(void) const {return(_myMessage);}
	void Client::clearMessage(void){this->_myMessage.clear();}
	const std::string & Client::getName(void) const { 
			return(this->getRequest().at("NICK").at(0));
	}
		const std::string & Client::getUserName(void) const { 
			return(this->getRequest().at("USER").at(1));
	}
	
void	Client::addNewChannel(std::string channelname)
{
	_myChannels.push_back(channelname);
}

void Client::setNick(const std::string nick){ _nick = nick;}
std::string Client::getNick() const{ return this->_nick;}
