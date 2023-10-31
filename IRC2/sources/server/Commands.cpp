#include "../../headers/server/Server.hpp"
#include "../../headers/channel/Channel.hpp"


	void	Server::caseSeparator(std::string &channelName, std::string &mode, size_t poz, size_t poz_end)
	{
		(void) poz_end;
		// we found separator at position poz
		mode = channelName.substr(0, poz);
		channelName = channelName.substr(poz + 1, channelName.size() - (poz));
	}

	void	Server::caseNoseparator(std::string &channelName, std::string &mode)
	{
		size_t	poz = 0;
		std::string substr;

		substr = channelName.substr(0, 1);

		if (!channelName.empty() && (poz = substr.find_first_of("#!&+")) != std::string::npos)
		{
			mode = substr;
			channelName = channelName.substr(1, channelName.size() - 1);
		}
	}

	std::string Server::stringTolower(std::string &str){
		int	i = 0;
		std::string lowerString;
		while (str.c_str()[i])
		{
			lowerString += std::tolower(str.c_str()[i]);
			i++;
		}
		return (lowerString);
	}

	bool	 Server::checkModError(Client &cl, std::vector<std::string> &listChannel){
		bool	ok = true;
		for (std::vector<std::string>::iterator itChan = listChannel.begin()
			; itChan != listChannel.end(); ++itChan)
		{
			if (itChan->size() != 1 && std::string(1, itChan->at(0)).find_first_of("#!+&") == std::string::npos) {
				lp_self(cl, getReplies(cl, 476, *itChan));
				ok = false;
			}
		}
		return (ok); 
	}

	bool	Server::checkChanError(Client &cl, std::vector<std::string> &listChannel) 
	{
		bool	ok = true;
		for (std::vector<std::string>::iterator itChan = listChannel.begin()
			; itChan != listChannel.end(); ++itChan)
		{
			if ((*itChan).size() > 50)
				lp_self(cl, getReplies(cl, 405, *itChan));
			else if((*itChan).find("^G") != std::string::npos)
				lp_self(cl, getReplies(cl, 403, *itChan));
			else
				continue ;
			ok = false;
		}
		return (ok);
	}

	bool Server::checkExistingChan(std::string chName)
	{
		for (std::vector<Channel>::iterator itChan = _channels.begin()
				; itChan != _channels.end(); ++itChan) {
				if (itChan->getName() == chName)
					return(true);
		}
		return(false);
	}

	void	Server::quitChan(Client &cl, Channel &chan) {
		if (!chan.getConnected().count(cl.getRequest().at("NICK").at(0)))
			return ;
		chan.setConnected().erase(cl.getRequest().at("NICK").at(0));
		lp_channel(cl, chan, "PART " + chan.getName() + " :" + cl.getRequest().at("NICK").at(0) + " left the channel.");
	}

	Channel *Server::getChanByName(const std::string &nameOfChannel)
	{
		for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++){
			if ( (*it).getName() == nameOfChannel)
				return (&(*it));
		}
		std::cout << "No corresponding channel found in _channel getchanbyname() " << std::endl; // DEBUG DELETE BEFORE PUSHING
		return (NULL);
	}

	std::map<std::string, std::string> Server::fill_map(std::vector<std::string> listChannel, std::vector<std::string> listPassword)
	{
		std::map<std::string, std::string> map;
		std::vector<std::string>::iterator Namepass = listPassword.begin();
		for (std::vector<std::string>::iterator NameChan = listChannel.begin(); NameChan != listChannel.end(); ++NameChan)
		{
			if (Namepass != listPassword.end())
			{
				map.insert(std::make_pair(*NameChan, *Namepass));
				++Namepass;
			}
			else
				map.insert(std::make_pair(*NameChan, ""));
		}
		for (std::map<std::string, std::string>::iterator mapit = map.begin(); mapit != map.end(); ++mapit)
		{
			std::cout << "chanName= " << (mapit->first) << "chanpass= " << mapit->second << std::endl;
			if (mapit->second.empty())
				std::cout << " for channel : " <<  mapit->first << "password is empty" << std::endl;
		}
		return (map);
	}

void	Server::create_new_channel(std::string const & channel_name, std::string const &channel_pass, Client &client)
{
	std::cout <<  "create new channel()" << std::endl;
	Channel new_channel = Channel(channel_name, channel_pass);
	//'#','+','&','!';
	std::string client_name = client.getName();
	new_channel.setChanAdmin(client_name);
	new_channel.setChanAdmin(this->_admin);
	new_channel.addConnectedUser(client_name);
	std::cout << "New channel infos - channel_name - password - admin :  " << channel_name << "," << channel_pass << "," << client_name << std::endl;
	this->_channels.push_back(new_channel);
	client.addNewChannel(channel_name);
}

void	Server::join_existing_channel(Channel *channel_tojoin, Client &client)
{
	std::string chan = channel_tojoin->getName();
	std::cout <<  "Join existing channel()" << std::endl;
	std::string client_name = client.getName();
		channel_tojoin->addConnectedUser(client_name);
		client.addNewChannel(channel_tojoin->getName());
}



void	Server::execJoin(Client &cl, std::vector<std::string> &listChannel, std::vector<std::string> &listPassword) 
{
	// (void) listPassword;
	// (void) cl;
	// (void) listChannel;
	std::string name_current_channel;
	int			join_status;
	std::map<std::string, std::string> map = fill_map(listChannel, listPassword);

	if ( map.size() == 1  && map.begin()->first == "0")
		join_reply(QUIT_ALL_CHANNELS, cl, "");
	else
	{
		std::map<std::string, std::string>::iterator map_iterator = map.begin();
		for (map_iterator = map.begin(); map_iterator != map.end(); ++map_iterator)
		{
			name_current_channel = map_iterator->first;
			if ((checkExistingChan(map_iterator->first) == false)) // CHANNEL NOT EXIST
			{
				std::cout << "case new channel: " << name_current_channel << std::endl;
				create_new_channel(name_current_channel, map_iterator->second, cl); // passing name of new channel and corresponding password.
				// reply comes here
				join_reply(JOIN_NEW_CHANNEL_OK, cl, name_current_channel);
			}	
			else // CHANNEL EXIST
			{
				std::cout << "case already existing channel: " << name_current_channel << std::endl;
				Channel *toJoin= getChanByName(map_iterator->first); // this is a Server:: function so it has access to vector of channels : _channels;
				if (toJoin== NULL)
					perror("Channel not found in vector channel");
				join_status = user_can_join(cl, *toJoin, map_iterator->second);
				if (join_status == GOOD_PASSWORD || join_status == INVITED) // client entered good password to join this channel
				{
					join_existing_channel(toJoin, cl);
					join_reply(JOIN_EXISTING_CHANNEL_OK, cl, name_current_channel);
				}
				else // password entered by client is incorrect 
				{
					if (join_status == BAD_PASSWORD)
						join_reply(BAD_PASSWORD, cl, name_current_channel);
					else if (join_status == NOT_INVITED)
						join_reply(NOT_INVITED, cl, name_current_channel);
					else
						join_reply(CHANNEL_FULL, cl, name_current_channel);
				}
			}
		}
	}
}
				// std::cout << cl.getRequest().at("NICK").at(0) << std::endl;
	// }
			// if (itNamePass + 1 != listPassword.end())
			// 	++itNamePass;
			// for (std::vector<Channel>::iterator itChan = _channels.begin()
			// 	; itChan != _channels.end(); ++itChan) {
			// 	std::cout << "ETAPE 1,5" << std::endl;
			// 	if (itNameChan->at(0) == '0'){
			// 		// for ()
			// 			std::cout << "ETAPE 0" << std::endl;
			// 			quitChan(cl, *itChan);//rajouter la boucle.
			// 	}
			// 	if (*itNameChan == itChan->getName()) {
			// 		std::cout << "ETAPE 2" << std::endl;
			// 		if (itChan->getNbr() != 0 && itChan->getConnected().size() >= itChan->getNbr())
			// 			lp_self(cl, getReplies(cl, 471, itChan->getName()));
			// 		else if ((itChan->getPassword().empty() == true
			// 			&& itNamePass != listPassword.end())
			// 			|| (itChan->getPassword().empty() == false
			// 			&&	(itNamePass == listPassword.end()
			// 			|| itChan->getPassword() != *itNamePass)))
			// 				lp_self(cl, getReplies(cl, 475, itChan->getName()));
			// 		else if (itChan->getRights() == Channel::INVITE_ONLY
			// 			&& itChan->inviteStatus(cl.getRequest().at("NICK").at(0)) == false){
			// 			lp_self(cl, getReplies(cl, 473, itChan->getName()));
			// 			std::cout << "ETAPE 3" << std::endl;
			// 		}
			// 		else {
						// std::cout << "Je join un channel qui existe deja" << std::endl;
						// itChan->setConnected(cl.getRequest().at("NICK").at(0));
						// lp_self(cl, SERVER_NAME + itChan->getName() + " :" + cl.getRequest().at("NICK").at(0) + " join the channel.");
						// lp_self(cl, getReplies(cl, 332, itChan->getName(), itChan->getTopic()));
						// std::string listConnectedUser;
						// for (std::set<std::string>::iterator itUser = itChan->getConnected().begin(); itUser != itChan->getConnected().end(); ++itUser)
						// // 	std::cout << *itUser << std::cout;
						// 	listConnectedUser += *itUser + " ";
						// std::string mod = itChan->getName().substr(0, 1);
						// lp_self(cl, getReplies(cl, 353, mod , itChan->getName(), listConnectedUser));
						// std::string msg = ": End of LIST\r\n";
						// lp_self(cl, msg);
					// }
				// if (itNameChan == listChannel.end()){
				// 	std::cout << "ETAPE 5" << std::endl;
				// 	itChan->setConnected(cl.getRequest().at("NICK").at(0));
				// 	std::cout << "Je suis dans channel nouveau" << std::endl;
				// 	// lp_self(cl, SERVER_NAME + itChan->getName() + " :" + cl.getRequest().at("NICK").at(0) + " join the channel.");
				// 	lp_self(cl, "Welcome to the channel " + itChan->getName() + ".\r\n");
				// 	lp_self(cl, getReplies(cl, 332, itChan->getName(), itChan->getTopic()));
				// 	std::string listConnectedUser;
				// 	for (std::set<std::string>::iterator itUser = itChan->getConnected().begin(); itUser != itChan->getConnected().end(); ++itUser)
				// 	// 	std::cout << *itUser << std::cout;
				// 		listConnectedUser += *itUser + " ";
				// 	std::string mod = itChan->getName().substr(0, 1);
				// 	lp_self(cl, getReplies(cl, 353, mod , itChan->getName(), listConnectedUser));
				// 	std::string msg = ": End of LIST\r\n";
				// 	lp_self(cl, msg);
				// 	}
				// break ;
			// }
		// }
	// 	}
	// }

	void	Server::channelJoin(Client &cl) 
	{
		std::cout << "channelJoin function called!" << std::endl;
		if (cl.getRequest().at("JOIN").size() == 0) {
			// lp_self(cl, getReplies(cl, 461, "JOIN"));
			return;
		}
		std::string request = cl.getRequest().at("JOIN").at(0);
		for (size_t pos = request.find(":"); pos != std::string::npos; pos = request.find(":", pos))
   			 request.erase(pos, 1);
			cl.setRequest().at("JOIN").at(0) = request;

		// std::cout << __FUNCTION__ << " : " << __LINE__ << std::endl;

		std::vector<std::string> 	listChannel = split(cl.getRequest().at("JOIN").at(0), ",");
		std::vector<std::string> 	listPassword;
		if (cl.getRequest().at("JOIN").size() > 1)
			listPassword = split(cl.getRequest().at("JOIN").at(1), ",");
		for (std::vector<std::string>::iterator itChan = listChannel.begin(); itChan != listChannel.end()
			; itChan++)
			std::cout << *itChan << std::endl;
		for (std::vector<std::string>::iterator itChan = listPassword.begin(); itChan != listPassword.end()
			; itChan++)
			std::cout << *itChan << std::endl;

		if (checkModError(cl, listChannel) == false
			|| checkChanError(cl, listChannel) == false)
			return ;

		execJoin(cl, listChannel, listPassword);


		// (void) ret;
		// // std::cout << " getReq JOIN: " << cl.getRequest().at("/JOIN") << std::endl;

		// parsJoin(listChannel, listPassword, listMode, cl.getRequest().at("/JOIN")); //
		// int msgError;
		// std::string msg;
		// if ((msgError = checkModError(listMode)) != OK){
		// 	msg = createErrorMsg(msgError);
		// 	lp_self(cl, msg);
		// 	std::cout << "ERR MODE " << std::endl;
		// 	std::cout << msg << " : msg " << std::endl;
		// }
		// if ((msgError = checkChanError(listChannel)) != OK){
		// 	msg = createErrorMsg(msgError);
		// 	lp_self(cl, msg);
		// }
	}

		//addJoinedChannelsToclient(cl, listChannel, listMode, listPassword);
		//addJoinedChannelsToclient(cl, listChannel, listMode, listPassword);

		// std::cout << "Channel : [" << listChannel[0] << "] , Password : [" << password
	// 	for (std::vector<std::string>::iterator ch = listChannel.begin(); ch != listChannel.end(); ++ch){
	// 		std::vector<Channel>::iterator it;
	// 		for (it = _channels.begin(); it != _channels.end(); ++it){
	// 			// std::cout << "Channel1 : " << it->getName() << " size : " << it->getName().size() << std::endl;
	// 			if (ch->empty()){
	// 				std::string message = "Please see /JOIN usage.\r\n";
	// 				lp_self(cl, message);
	// 				break ;
	// 			}
	// 			if (*ch == it->getName()) {
	// 				// std::cout << "PASS saved : " << it->getPassword() << std::endl;
	// 				// std::cout << "PASS CHECKED : " << password << std::endl;
	// 				if (it->getPassword() != password){
	// 					std::string message = *ch + ": wrong password\r\n";
	// 					lp_self(cl, message);
	// 					break;
	// 				} // rajouter les invites
	// 				else if ((!it->getNbr() || it->getConnected().size() < it->getNbr())) {
	// 					it->setList().insert(std::make_pair(cl.getRequest().at("NICK"), Channel::regular_user));
	// 					it->setConnected().insert(cl.getRequest().at("NICK"));
	// 					std::string message1 = "Je suis connecte au channel";
	// 					std::string message2 = "Un client vient d arriver sur le server";
	// 					lp_self(cl, message1);
	// 					lp_channel(cl, *it, message2);
	// 					// lp_server(cl, server);
	// 					//modifier en fonction des droits
	// 					break;
	// 				}
	// 				else{
	// 					std::string message = *ch + ": limit of connected user\r\n";
	// 					lp_self(cl, message);
	// 					break ;
	// 				}
	// 				break;
	// 			}
	// 		}
	// 		if (it == _channels.end()){
	// 			// std::cout << "Channel1 : " << ch->empty() << std::endl;
	// 			if (ch->empty()){
	// 				std::string message = "Please see /JOIN usage.\r\n";
	// 				lp_self(cl, message);
	// 				break ;
	// 			}
	// 			_channels.push_back(Channel(*ch, password));
	// 			std::cout << "Channel : [" << _channels.back().getName() << "] , Password : [" << _channels.back().getPassword() << std::endl; 
	// 			// std::cout << "Channel2 : " << _channels.back().getName() << " size : " << _channels.back().getName().size() << std::endl;
	// 			_channels.back().setList().insert(std::make_pair(cl.getRequest().at("NICK"), Channel::operators));
	// 			_channels.back().setConnected().insert(cl.getRequest().at("NICK"));
	// 			cl.setMessage("Welcome to the channel " + *ch + ".\r\n" + "The topic of the channel is: " + _channels.back().getTopic() + "\r\n" + "List of connected user on this channel: ");
	// 			const std::map<std::string, Channel::ch_rights> &mp = _channels.back().getList();
	// 			for (std::set<std::string>::iterator it = _channels.back().getConnected().begin()
	// 				; it != _channels.back().getConnected().end(); ++it) {
	// 				cl.setMessage((mp.at(*it) == Channel::operators ? "@" + *it : *it ) + ".\r\n");
	// 			}
	// 			// cl.setMessage("You can use the following commands: /JOIN, /TOPIC, /PRIVMSG, /QUIT, /KICK, /INVITE, /MODE\r\n");
	// 			std::string message = "You can use the following commands: /JOIN, /TOPIC, /PRIVMSG, /QUIT, /KICK, /INVITE, /MODE\r\n";
	// 			lp_self(cl, message);
	// 			std::string message2 = cl.getRequest().at("NICK") + "has joined the channel.\r\n";
	// 			lp_channel(cl, _channels.back(), message2);
	// 		}
	// 	}
	 // 	return ;	
	 // }


	std::string Server::getReplies(Client &cl, unsigned short msgError, std::string arg1, std::string arg2, std::string arg3, std::string arg4, std::string arg5, std::string arg6, std::string arg7)
	{
		std::string target;
		if (cl.getStatus() == Client::disconnected)
			target = "*";
		else
			target = cl.getRequest().at("NICK").at(0);
		target += " ";
		switch (msgError)
		{
		case 001:
			return target + RPL_WELCOME(arg1);
		case 002:
			return target + RPL_YOURHOST(arg1, arg2);
		case 003:
			return target + RPL_CREATED(arg1);
		case 004:
			return target + RPL_MYINFO(arg1, arg2, arg3, arg4);
		case 005:
			return target + RPL_BOUNCE(arg1, arg2);
		case 200:
			return target + RPL_TRACELINK(arg1, arg2, arg3);
		case 201:
			return target + RPL_TRACECONNECTING(arg1, arg2);
		case 202:
			return target + RPL_TRACEHANDSHAKE(arg1, arg2);
		case 203:
			return target + RPL_TRACEUNKNOWN(arg1, arg2);
		case 204:
			return target + RPL_TRACEOPERATOR(arg1, arg2);
		case 205:
			return target + RPL_TRACEUSER(arg1, arg2);
		case 206:
			return target + RPL_TRACESERVER(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
		case 208:
			return target + RPL_TRACENEWTYPE(arg1, arg2);
		case 211:
			return target + RPL_STATSLINKINFO(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
		case 212:
			return target + RPL_STATSCOMMANDS(arg1, arg2);
		case 213:
			return target + RPL_STATSCLINE(arg1, arg2, arg3, arg4);
		case 214:
			return target + RPL_STATSNLINE(arg1, arg2, arg3, arg4);
		case 215:
			return target + RPL_STATSILINE(arg1, arg2, arg3, arg4);
		case 216:
			return target + RPL_STATSKLINE(arg1, arg2, arg3, arg4);
		case 218:
			return target + RPL_STATSYLINE(arg1, arg2, arg3, arg4);
		case 219:
			return target + RPL_ENDOFSTATS(arg1);
		case 221:
			return target + RPL_UMODEIS(arg1);
		case 241:
			return target + RPL_STATSLLINE(arg1, arg2, arg3);
		case 242:
			return target + RPL_STATSUPTIME();
		case 243:
			return target + RPL_STATSOLINE(arg1, arg2);
		case 244:
			return target + RPL_STATSHLINE(arg1, arg2);
		case 251:
			return target + RPL_LUSERCLIENT(arg1, arg2, arg3);
		case 252:
			return target + RPL_LUSEROP(arg1);
		case 253:
			return target + RPL_LUSERUNKNOWN(arg1);
		case 254:
			return target + RPL_LUSERCHANNELS(arg1);
		case 255:
			return target + RPL_LUSERME(arg1, arg2);
		case 256:
			return target + RPL_ADMINME(arg1);
		case 257:
			return target + RPL_ADMINLOC1(arg1);
		case 258:
			return target + RPL_ADMINLOC2(arg1);
		case 259:
			return target + RPL_ADMINEMAIL(arg1);
		case 261:
			return target + RPL_TRACELOG(arg1, arg2);
		case 262:
			return target + RPL_TRACEEND(arg1, arg2);
		case 263:
			return target + RPL_TRYAGAIN(arg1);
		case 300:
			return target + RPL_NONE();
		case 301:
			return target + RPL_AWAY(arg1, arg2);
		case 302:
			return target + RPL_USERHOST(arg1);
		case 303:
			return target + RPL_ISON(arg1);
		case 305:
			return target + RPL_UNAWAY();
		case 306:
			return target + RPL_NOWAWAY();
		case 311:
			return target + RPL_WHOISUSER(arg1, arg2, arg3, arg4);
		case 312:
			return target + RPL_WHOISSERVER(arg1, arg2, arg3);
		case 313:
			return target + RPL_WHOISOPERATOR(arg1);
		case 314:
			return target + RPL_WHOWASUSER(arg1, arg2, arg3, arg4);
		case 315:
			return target + RPL_ENDOFWHO(arg1);
		case 317:
			return target + RPL_WHOISIDLE(arg1, arg2);
		case 318:
			return target + RPL_ENDOFWHOIS(arg1);
		case 319:
			return target + RPL_WHOISCHANNELS(arg1, arg2);
		case 321:
			return target + RPL_LISTSTART();
		case 322:
			return target + RPL_LIST(arg1, arg2, arg3);
		case 323:
			return target + RPL_LISTEND();
		case 324:
			return target + RPL_CHANNELMODEIS(arg1, arg2, arg3);
		case 325:
			return target + RPL_UNIQOPIS(arg1, arg2);
		case 331:
			return target + RPL_NOTOPIC(arg1);
		case 332:
			return target + RPL_TOPIC(arg1, arg2);
		case 341:
			return target + RPL_INVITING(arg1, arg2);
		case 342:
			return target + RPL_SUMMONING(arg1);
		case 346:
			return target + RPL_INVITELIST(arg1, arg2);
		case 347:
			return target + RPL_ENDOFINVITELIST(arg1);
		case 348:
			return target + RPL_EXCEPTLIST(arg1, arg2);
		case 351:
			return target + RPL_VERSION(arg1, arg2, arg3);
		case 352:
			return target + RPL_WHOREPLY(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
		case 353:
			return target + RPL_NAMREPLY(arg1, arg2, arg3);
		case 364:
			return target + RPL_LINKS(arg1, arg2, arg3, arg4);
		case 365:
			return target + RPL_ENDOFLINKS(arg1);
		case 366:
			return target + RPL_ENDOFNAMES(arg1);
		case 367:
			return target + RPL_BANLIST(arg1, arg2);
		case 368:
			return target + RPL_ENDOFBANLIST(arg1);
		case 369:
			return target + RPL_ENDOFWHOWAS(arg1);
		case 371:
			return target + RPL_INFO(arg1);
		case 372:
			return target + RPL_MOTD(arg1);
		case 374:
			return target + RPL_ENDOFINFO();
		case 375:
			return target + RPL_MOTDSTART(arg1);
		case 376:
			return target + RPL_ENDOFMOTD();
		case 381:
			return target + RPL_YOUREOPER();
		case 382:
			return target + RPL_REHASHING(arg1);
		case 383:
			return target + RPL_YOURESERVICE(arg1);
		case 391:
			return target + RPL_TIME(arg1, arg2);
		case 392:
			return target + RPL_USERSSTART();
		case 393:
			return target + RPL_USERS();
		case 394:
			return target + RPL_ENDOFUSERS();
		case 395:
			return target + RPL_NOUSERS();

		case 401:
			return target + ERR_NOSUCHNICK(arg1);
		case 402:
			return target + ERR_NOSUCHSERVER(arg1);
		case 403:
			return target + ERR_NOSUCHCHANNEL(arg1);
		case 404:
			return target + ERR_CANNOTSENDTOCHAN(arg1);
		case 405:
			return target + ERR_TOOMANYCHANNELS(arg1);
		case 406:
			return target + ERR_WASNOSUCHNICK(arg1);
		case 407:
			return target + ERR_TOOMANYTARGETS(arg1);
		case 408:
			return target + ERR_NOSUCHSERVICE(arg1);
		case 409:
			return target + ERR_NOORIGIN();
		case 411:
			return target + ERR_NORECIPIENT(arg1);
		case 412:
			return target + ERR_NOTEXTTOSEND();
		case 413:
			return target + ERR_NOTOPLEVEL(arg1);
		case 414:
			return target + ERR_WILDTOPLEVEL(arg1);
		case 415:
			return target + ERR_BADMASK(arg1);
		case 421:
			return target + ERR_UNKNOWNCOMMAND(arg1);
		case 422:
			return target + ERR_NOMOTD();
		case 423:
			return target + ERR_NOADMININFO(arg1);
		case 424:
			return target + ERR_FILEERROR(arg1, arg2);
		case 431:
			return target + ERR_NONICKNAMEGIVEN();
		case 432:
			return target + ERR_ERRONEUSNICKNAME(arg1);
		case 433:
			return target + ERR_NICKNAMEINUSE(arg1);
		case 436:
			return target + ERR_NICKCOLLISION(arg1);
		case 441:
			return target + ERR_USERNOTINCHANNEL(arg1, arg2);
		case 442:
			return target + ERR_NOTONCHANNEL(arg1);
		case 443:
			return target + ERR_USERONCHANNEL(arg1, arg2);
		case 444:
			return target + ERR_NOLOGIN(arg1);
		case 445:
			return target + ERR_SUMMONDISABLED();
		case 446:
			return target + ERR_USERSDISABLED();
		case 451:
			return target + ERR_NOTREGISTERED();

		case 461:
			return target + ERR_NEEDMOREPARAMS(arg1);
		case 462:
			return target + ERR_ALREADYREGISTRED();
		case 463:
			return target + ERR_NOPERMFORHOST();
		case 464:
			return target + ERR_PASSWDMISMATCH();
		case 465:
			return target + ERR_YOUREBANNEDCREEP();
		case 467:
			return target + ERR_KEYSET(arg1);
		case 471:
			return target + ERR_CHANNELISFULL(arg1);
		case 472:
			return target + ERR_UNKNOWNMODE(arg1);
		case 473:
			return target + ERR_INVITEONLYCHAN(arg1);
		case 474:
			return target + ERR_BANNEDFROMCHAN(arg1);
		case 475:
			return target + ERR_BADCHANNELKEY(arg1);
		case 476:
			return target + ERR_BADCHANMASK(arg1);
		case 477:
			return target + ERR_NOCHANMODES(arg1);
		case 478:
			return target + ERR_BANLISTFULL(arg1);
		case 481:
			return target + ERR_NOPRIVILEGES();
		case 482:
			return target + ERR_CHANOPRIVSNEEDED(arg1);
		case 483:
			return target + ERR_CANTKILLSERVER();
		case 484:
			return target + ERR_RESTRICTED();
		case 485:
			return target + ERR_UNIQOPPRIVSNEEDED();
		case 491:
			return target + ERR_NOOPERHOST();

		case 501:
			return target + ERR_UMODEUNKNOWNFLAG();
		case 502:
			return target + ERR_USERSDONTMATCH();
		default:
			return std::string();
		}
	}
