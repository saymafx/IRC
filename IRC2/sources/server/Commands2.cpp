# include "../../headers/server/Server.hpp"

	bool Server::execTopicAllowed(Client &cl, std::string channel) {
		Channel *toJoin= getChanByName(channel);
		if (toJoin->isOperator(cl.getName()) == OPERATOR)
			return true;
		else if (toJoin->getMode(TOPIC_RESTRICT_MODE, "") == UNSET && toJoin->isOperator(cl.getName()) != OPERATOR)
			return true;
		else
			return false;
	}

	void Server::execTopic(Client &cl, std::string channel, std::string newtopic){
		//size_t pos = newtopic.find("\"\"");
		//std::string check = """";
		Channel *toJoin= getChanByName(channel);
		if (getChanByName(channel) == NULL)
			lp_self(cl, getReplies(cl, 442, channel));
		else {
			 if ((toJoin->getTopic() == "" && newtopic.size() == 0) || (newtopic == "\"\"")) {
				if (execTopicAllowed(cl, channel) == true){
					toJoin->setTopic("");
					std::string msg = std::string(":") + SERVER_NAME + " 332 " + " : " +  toJoin->getName() + " : " + toJoin->getTopic();
					lp_channel(cl, *toJoin, msg);
					lp_self(cl, msg);
				}
				else{
					std::string msg = std::string(":") + SERVER_NAME + " 482 " + " : " +  toJoin->getName() + " : " + "You're not channel operator\r\n";
					lp_self(cl, msg);
				}

			}
			else if (newtopic.size() > 0 && (execTopicAllowed(cl, channel) == true) && channel.size() != 0) {

				toJoin->setTopic(newtopic);
				std::string msg = std::string(":") + SERVER_NAME + " 332 " + " : " +  toJoin->getName() + " : " + toJoin->getTopic();
				lp_channel(cl, *toJoin, msg);
				lp_self(cl, msg);
			}
			else if (newtopic.size() > 0 && (execTopicAllowed(cl, channel) == false) && channel.size() != 0) {
					std::string msg = std::string(":") + SERVER_NAME + " 482 " + " : " +  toJoin->getName() + " : " + "You're not channel operator";
					lp_self(cl, msg);
			}
			else {
				std::string msg = std::string(":") + SERVER_NAME + " 332 " + " : " +  toJoin->getName() + " : " + toJoin->getTopic();
				lp_self(cl, msg);
			}
		}
	}

//rajouter l option t du MODE -----------------------------------POUR L INSTANT TOUS LES ADMINS PEUVENT CHANGER LE TOPIC---------------------------------------------------------
	void Server::channelTopic(Client &cl){
			std::vector<std::string> request = cl.getRequest().at("TOPIC");
			std::string newtopic;
			// std::cout <<  "size : " << request.size() << std::endl;
			if (cl.getRequest().at("TOPIC").size() == 0) {
				// lp_self(cl, getReplies(cl, 461, "TOPIC"));
				return;
			}
			for(std::vector<std::string>::iterator topic = request.begin(); topic != request.end(); ++topic){
				if (request.size() == 1)
					break;
				if (topic != request.begin() && topic + 1 != request.end())
					newtopic += *topic + " ";
				else if (topic + 1 == request.end())
					newtopic += *topic;
			}
			if (newtopic.size() > 0 && newtopic.at(0) == ':')
				newtopic = newtopic.substr(1, newtopic.size() - 1);
			// std::cout << "newtopic : [" << newtopic << "]" << std::endl;
			execTopic(cl, request.at(0), newtopic);
	}

	Client *Server::getClientByName(const std::string &nameOfClient)
	{
		for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it){
			if ( (*it)->getName() == nameOfClient)
				return ((*it));
		}
		// std::cout << "No corresponding channel found in _client getchanbyClient() " << std::endl; // DEBUG DELETE BEFORE PUSHING
		return (NULL);
	}

		void Server::channelPriv(Client &cl){

			Channel *toJoin = NULL;
			Client *receiver = NULL;
			std::vector<std::string> request = cl.getRequest().at("PRIVMSG");
			if (cl.getRequest().at("PRIVMSG").size() == 0) {
				// lp_self(cl, getReplies(cl, 461, "PRIVMSG"));
				return;
			}
			// std::cout <<  "request " << request.at(0) << std::endl;
			if (request.size() > 0){
				toJoin= getChanByName(cl.getRequest().at("PRIVMSG").at(0));
				receiver = getClientByName(cl.getRequest().at("PRIVMSG").at(0));
			}
			std::string newmessage;
			// std::cout <<  "size : " << request.size() << std::endl;
			for(std::vector<std::string>::iterator newmsg = request.begin(); newmsg!= request.end(); ++newmsg){
				if (request.size() == 1)
					break;
				if (newmsg != request.begin() && newmsg + 1 != request.end())
					newmessage += *newmsg + " ";
				else if (newmsg + 1 == request.end())
					newmessage += *newmsg;
			}
			if (!newmessage.empty()) {
				if (newmessage.at(0) == ':')
					newmessage = newmessage.substr(1, newmessage.size() - 1);
			}
			else {
				lp_self(cl, getReplies(cl, 409, "PRIVMSG"));
				return;
			}
			// std::cout <<  "newmessage " << newmessage << std::endl;
			if (request.size() == 0)
				lp_self(cl, getReplies(cl, 411, "PRIVMSG"));
			else if (request.size() == 1 && toJoin != NULL)
				lp_self(cl, getReplies(cl, 412));
			else if (toJoin == NULL && receiver == NULL)
				lp_self(cl, getReplies(cl, 411, "PRIVMSG"));
			else if (toJoin != NULL && receiver == NULL ){
				std::string msg = std::string(":") + cl.getName() + " PRIVMSG " + toJoin->getName() + " : " + newmessage;
				lp_channel(cl, *toJoin, msg);
			}
			else if(receiver != NULL && toJoin == NULL){
				std::string msg = std::string(":") + cl.getName() + " PRIVMSG " + ": " + newmessage;
				lp_private(cl, receiver->getName(), msg);
			}
	}

	void Server::channelQuit(Client &cl){
		std::vector<std::string> request = cl.getRequest().at("QUIT");
		std::string msg = "ERROR";
		lp_self(cl, msg);
	}

	// void	Server::kickChan(Client &cl, Channel &chan) {
	// 	if (!chan.getConnected().count(cl.getRequest().at("NICK").at(0)))
	// 		return ;
	// 	chan.setConnected().erase(cl.getRequest().at("NICK").at(0));
	// 	lp_channel(cl, chan, " PART " + chan.getName() + " :" + cl.getRequest().at("NICK").at(0) + " has been kicked.");
	// }

	void Server::channelKick(Client &cl)
	{
		// (void) cl;
		if (cl.getRequest().at("KICK").size() <= 2){
			lp_self(cl, getReplies(cl, 461, "KICK"));
			return ;
		}

		std::string request = cl.getRequest().at("KICK").at(1);
		std::string newmessage;
		std::string msg;
		Channel *toKick = NULL;
		for (size_t pos = request.find(":"); pos != std::string::npos; pos = request.find(":", pos))
   			 request.erase(pos, 1);
			cl.setRequest().at("KICK").at(1) = request;
		request = cl.getRequest().at("KICK").at(2);
		for (size_t pos = request.find(":"); pos != std::string::npos; pos = request.find(":", pos))
   			 request.erase(pos, 1);
			cl.setRequest().at("KICK").at(2) = request;
		std::vector<std::string> 	listChannel = split(cl.getRequest().at("KICK").at(1), ",");
		std::vector<std::string> 	listUsers = split(cl.getRequest().at("KICK").at(2), ",");
		if (cl.getRequest().at("KICK").size() > 3){
			size_t i = 3;
			while (i != cl.getRequest().at("KICK").size()){
				if (i + 1 != cl.getRequest().at("KICK").size())
					newmessage += cl.getRequest().at("KICK").at(i) + " ";
				else
					newmessage += cl.getRequest().at("KICK").at(i);
				i++;
			}
		}
		else
			newmessage = "get kicked";
		std::cout << "size channel : " << listChannel.size() << "size userlist :" <<  listUsers.size() << std::endl;
		int i = listChannel.size();
		while (i >= 1)
		{
			toKick = getChanByName(listChannel.at(i - 1));
			if (listChannel.size() >= 1 && toKick != NULL && toKick->isOperator(cl.getName()) == OPERATOR){
				std::cout << "size channel : " << listChannel.size() << "size userlist :" <<  listUsers.size() << std::endl;
					for(std::vector<std::string>::iterator it = listUsers.begin(); it != listUsers.end(); ++it){
						if(toKick->isConnected(*it) == true){
							Client *self = getClientByName(*it);
							toKick->setConnected().erase(*it);
							std::string msg = std::string(":") + cl.getName() + " KICK " + toKick->getName() + " " + self->getName() + " :" + newmessage;
							lp_private(cl, self->getName(), msg);
							std::string listConnectedUser;
							for (std::set<std::string>::iterator itUser = toKick->getConnected().begin(); itUser != toKick->getConnected().end(); ++itUser)
							{
								if (_channels.back().isOperator(*itUser) == OPERATOR) // fonction qui determine si un client est opérateur dans un channel donné.
									listConnectedUser += "@";
								listConnectedUser += *itUser + " ";
							}
							std::cout << "les gens connecté sur : " << listChannel.at(0) << " : " << listConnectedUser << std::endl;
							msg = std::string(":") + SERVER_NAME + std::string(" 353 ") + cl.getRequest().at("NICK").at(0) + " = " + toKick->getName() + " :" + listConnectedUser + "\r\n";
    						//std::cout << "MESSAGE :" << msg << std::endl;
    						lp_self(cl, msg);
    						lp_channel(cl, *toKick, msg);
    						msg = std::string(":") + SERVER_NAME + std::string(" 366 ") + cl.getRequest().at("NICK").at(0) + " " + toKick->getName() + " :End of NAMES list";
    						lp_self(cl, msg);
    						lp_channel(cl, *toKick, msg);
						}
						else{
							msg = std::string(":") + SERVER_NAME + std::string(" 441 ") + cl.getRequest().at("NICK").at(0) + toKick->getName() + ": User not in Channel";
							lp_self(cl, msg);
						}
					}
			}
			else if (listChannel.size() == 1 && toKick != NULL && toKick->isOperator(cl.getName()) != OPERATOR)
			{
				newmessage = std::string(":") + SERVER_NAME + std::string(" 482 ") + cl.getRequest().at("NICK").at(0) + toKick->getName() + ": You're not channel operator" "\r\n";
    			lp_self(cl, newmessage);
					// ERR_CHANOPRIVSNEEDED
			}
			i--;
	}

}

void Server::channelInvit(Client &cl)
	{
		if (cl.getRequest().at("INVITE").size() == 0) {
				// lp_self(cl, getReplies(cl, 461, "INVITE"));
				return;
		}		
		std::string UserToInvite = cl.getRequest().at("INVITE").at(0);
		std::string InvitChan = cl.getRequest().at("INVITE").at(1);
		std::string msg;
		Channel *chanInvit = getChanByName(InvitChan);
		Client *self = getClientByName(UserToInvite);
		if (chanInvit == NULL){
			lp_self(cl, ERR_NOSUCHCHANNEL(InvitChan));
			return;
		}
		else if (self == NULL){
			lp_self(cl, ERR_NOSUCHNICK(UserToInvite));
			return;
		}
		else if(chanInvit->isConnected(UserToInvite)){
			lp_self(cl, ERR_USERONCHANNEL(UserToInvite, InvitChan));
			return;
		}
		else if (chanInvit != NULL && chanInvit->isOperator(cl.getName()) == OPERATOR){
			chanInvit->setInvite(UserToInvite);
			lp_private(cl, UserToInvite, getReplies(cl, 341, "invited you to " + InvitChan));
			lp_self(cl, getReplies(cl, 341, "invited " + UserToInvite + " to " + InvitChan));
			std::cout << "Invit sent" << std::endl;
		}
		else if (chanInvit != NULL && chanInvit->isOperator(cl.getName()) != OPERATOR){
				msg = std::string(":") + SERVER_NAME + std::string(" 482 ") + cl.getRequest().at("NICK").at(0) + InvitChan + ": You're not channel operator";
    			lp_self(cl, msg);
		}



}

// bool	Server::channelExists(const std::string& channelName) const {
// 		return std::find_if(_channels.begin(), _channels.end(), ChannelComparator(channelName)) != _channels.end();
//     }

bool	Server::CharOutOfRange(char c) {
	if (c != 'i' && c != 't' && c != 'k' && c != 'o' && c != 'l' && c != '+' && c != '-')
		return true;
	else
		return false;
}

int		Server::findOpt(char c) {
	if (c == 'i')
		return INVITE_ONLY_MODE;
	else if (c == 't')
		return TOPIC_RESTRICT_MODE;
	else if (c == 'k')
		return PASSWORD;
	else if (c == 'o')
		return PRIVILEGE;
	else if (c == 'l')
		return LIMIT;
	else
		return 0;
}


void	Server::channelMode(Client &cl) {
	iterator_map it = cl.getRequest().begin();
	std::string channelName = it->second.at(0);
	std::string 	msg;
	Channel *chan = this->getChanByName(channelName);
	//If used incorrectly (size = 0 will never be true since Xchat passes name of channel as argument automatically when used with no args)
	if (it->second.size() == 0) {
		// lp_self(cl, getReplies(cl, 461, "MODE"));
		return;
	}
	if  (it->second.size() == 1) {
		std::string currentModes = "";
		std::string currentModeParams;
		Channel *channel = getChanByName(channelName);
		if (channel->getMode(INVITE_ONLY_MODE, "") == SET)
			currentModes +=  "i";
		if (channel->getMode(TOPIC_RESTRICT_MODE, "") == SET)
			currentModes +=  "t";
		if(channel->getNbr() != 0) {
			currentModes += "l";
			std::stringstream ss;
			ss << channel->getNbr();
			currentModeParams += " " + ss.str();
		}
		if (channel->getMode(PASSWORD, "") == SET) {
			currentModes += "k";
			currentModeParams += " " + channel->getPassword();
		}
		msg = std::string(":") + SERVER_NAME + " 324 " +  cl.getName() + " " + chan->getName() + " +" + currentModes + currentModeParams;
		lp_self(cl, msg);
		/* 
		std::string msg = std::string(":") + SERVER_NAME + " 332 " + " : " +  toJoin->getName() + " : " + toJoin->getTopic();
					lp_channel(cl, *toJoin, msg);
					lp_self(cl, msg);
		*/
	}
	//execute /Mode w args
	else {
		if (!(chan->isOperator(cl.getName()) == OPERATOR)) {
		msg = std::string(":") + SERVER_NAME + " 482 " +  cl.getName() + " " + chan->getName() + "  :You're not channel operator";
		lp_self(cl, msg);
		return;
		}
		execModes(cl, *chan);
	}
	return;		//
}

void	Server::execModes(Client &cl, Channel &chan) {
	(void)chan;
	iterator_map it = cl.getRequest().begin();
	std::string channelName = it->second.at(0);
	Channel *channel = getChanByName(channelName);
	std::string ModesReq = it->second.at(1);
	int len = ModesReq.size();
	char curSign = '0';

	int mode;
	int action;
	int limit = -2;
	int paramOfMode = 2;
	std::string curModeParams = "";
	for (int i = 0; i < len; i++) {
		if (ModesReq.at(i) == '+' || ModesReq.at(i) == '-') {
			curSign = ModesReq.at(i);
			continue;
		}
		else if (CharOutOfRange(ModesReq.at(i))) {
			lp_self(cl, getReplies(cl, 472, std::string(1, ModesReq.at(i))));
			continue;
		}
		else {
			if (curSign == '0')
				continue;
			action = (curSign == '+') ? SET : UNSET;
			mode = findOpt(ModesReq.at(i));
			if (mode == LIMIT || mode == PASSWORD || mode == PRIVILEGE) {
				if (paramOfMode >= static_cast<int>(it->second.size()) && action == SET)
					continue;
				if (mode == PASSWORD && action == SET) {
					curModeParams = it->second.at(paramOfMode);
				}
				else if(mode == LIMIT && action == SET) {
						limit = std::atoi(it->second.at(paramOfMode).c_str());
						if (action == SET && limit <= 0)
							continue;
				}
				else if(mode == PRIVILEGE) {
					 if (paramOfMode >= static_cast<int>(it->second.size())) {
						// Handle the case where paramOfMode is out of range
						std::string msg0 = "Need more parameters";
						lp_self(cl, msg0);
						continue;
					}
					curModeParams = it->second.at(paramOfMode);
					if (!channel->isConnected(curModeParams)) {
						std::string msg0 = curModeParams + " :No such nick/channel";
						lp_self(cl, msg0);
						continue;
					}
				}
				paramOfMode++;
			}
			int ret = channel->setMode(mode, action, limit, curModeParams, channelName, cl.getName());
			if (ret == 1) {
				std::string actstr = (action == SET) ? "+" : "-";
				std::string modstr = modeToString(mode);
				std::string msg = ":" + cl.getName() + "!~" + cl.getUserName() + "@host MODE " + chan.getName() + " " + actstr + modstr;
				if (limit > 0 && modeToString(mode) == "l"){
					std::ostringstream oss;
					oss << limit;
					std::string limitStr = oss.str();
					msg += " " + limitStr;
				}
				if (curModeParams != " " && mode == PASSWORD) {
					msg += " " + curModeParams;
				}
				if (curModeParams != " " && mode == PRIVILEGE) {
					msg += " " + curModeParams;
				}
				lp_self(cl, msg);
				lp_channel(cl, chan, msg);
			}

		}
	}
}