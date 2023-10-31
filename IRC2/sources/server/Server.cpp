# include "../../headers/server/Server.hpp"
# include "../../headers/client/Client.hpp"
# include "../../headers/channel/Channel.hpp"

	Server::Server(const std::string & addr, const unsigned short port, const std::string password) : Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), 
		_addr(addr), _password(password), _port(port), _admin("admin") {
			static const int    optval = true;
			setSocket(AF_INET, _addr.c_str(), _port);
			setSockOpt(SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
			bindSocket();
			listenSocket(MAX_LISTEN);
		}

	Server::~Server(){
		for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
			delete *it;
	}

	std::vector<std::string>	Server::split(std::string line, std::string delimiteur) {
		std::vector<std::string> split;
		size_t	pos;
		while ((pos = line.find(delimiteur)) != std::string::npos) {
			split.push_back(line.substr(0, pos));
			line.erase(0, pos + delimiteur.size());
		}
		if (!line.empty())
			split.push_back(line);
		return (split);
	}
	std::string Server::getCurrentDate()
	{
	    std::time_t now = std::time(0);

	    std::tm* localTime = std::localtime(&now);
	    char buffer[100];
	    std::strftime(buffer, sizeof(buffer), "%a %b %d %T %Y", localTime);
	    return std::string(buffer);
	}

	std::string Server::trimEndNewline(const std::string& s) {
		std::string result = s;
		
		if (!result.empty() && result[result.size() - 1] == '\n' && result[result.size() - 2] != '\r') {
			// Check if it's not preceded by '\r'
			if (result.size() == 1 || (result.size() > 1 && result[result.size() - 2] != '\r')) {
				result.erase(result.size() - 1);
			}
		}
		return result;
	}



	void Server::getRequest(const char * str, std::map<std::string, std::vector<std::string> > &request, 
		const Client &cl)
	{
		static const char	*saveMethod[] = {"SERVER", "CAP", "NICK", "REAL", "USER", "PASS", NULL};
		// static const char	*command[] = {"/JOIN", "/TOPIC", "/PRIVMSG", "/QUIT", "/KICK", "/INVITE", "/MODE", NULL};
		std::string 		line(str);

		if (line.find_first_not_of(" \t\r\n") == std::string::npos)
        	return;  // If empty or only whitespace, return without processing
		//line = trimEndNewline(line);
		(void)cl;
		put_line(line);
		if (cl.getStatus() != Client::disconnected) {
			for (iterator_map it = request.begin(); it != request.end(); ++it) {
				int	i = 0;
				for (; saveMethod[i]; ++i)
					if (it->first == saveMethod[i])
						break ;
				if (!saveMethod[i]) {
					request.erase(it->first);
					it = request.begin();
					if (request.empty())
						break ;
				}
		 	}
		}
		else {
			static const char	*saveMethod2[] = {"SERVER", "CAP", "NICK", "REAL", "USER", "PASS", "JOIN", "TOPIC", "PRIVMSG", "QUIT", "KICK", "INVITE", "MODE", NULL};
			for (iterator_map it = request.begin(); it != request.end(); ++it) {
				int	i = 0;
				for (; saveMethod2[i]; ++i)
					if (it->first == saveMethod2[i])
						break ;
				if (!saveMethod2[i]) {
					request.erase(it->first);
					it = request.begin();
					if (request.empty())
						break ;
				}
		 	}
		}
		std::string delimiter;
		if (line.find("\r\n") != std::string::npos)
			delimiter = "\r\n";
		else
			delimiter = "\n";
		std::vector<std::string> line_split = split(line, delimiter);
		for (std::vector<std::string>::const_iterator itLineSplit = line_split.begin();
		itLineSplit != line_split.end(); ++itLineSplit) {
			std::vector<std::string> line_split_with_space = split(*itLineSplit, " ");
			
			if(line_split_with_space.empty()) {
				continue; // Skip the current iteration if the vector is empty
			}
			std::string key = line_split_with_space.at(0);
			if (key == "USER" && line_split_with_space.size() > 4) {
				std::size_t colon_pos = line_split_with_space[4].find(":");
				if (colon_pos != std::string::npos) {
					for (std::size_t i = 5; i < line_split_with_space.size(); ++i) {
						line_split_with_space[4] += " " + line_split_with_space[i];
					}
					line_split_with_space.resize(5);  // trim the vector to only have four arguments now
				}
			}
			if ((key == "NICK" || key == "PASS" || key == "USER") && cl.getStatus() != Client::disconnected)
				continue;
			line_split_with_space.assign(line_split_with_space.begin() + 1, line_split_with_space.end());
			// Check if the key already exists
			if (request.find(key) != request.end()) {
				// Key exists, overwrite
				request[key] = line_split_with_space;
			} else {
				// Key doesn't exist, insert
				request.insert(std::make_pair(key, line_split_with_space));
			}
			
		}

		std::cout << " ---------------- BEGIN OF REQUEST --------------------" << std::endl;
		for (iterator_map it = request.begin(); it != request.end(); ++it) {
			std::cout << "["; put_line(it->first); std::cout << "]:";
			for (std::vector<std::string>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
				std::cout << " ["; put_line(*it2); std::cout << "]";
			}
			std::cout << std::endl;
		}	
		std::cout << " ---------------- END OF REQUEST -------------------- \n" << std::endl;
	}
	// :127.0.0.1 464 * :Authentication failed. Connection refused.\r\n
	// "001" : Renvoyé lorsqu'un Client s'est correctement connecté et est le premier message de bienvenue.

	bool	Server::check_connection_error( Client &cl ) {
		// PASSWORD CHECK
		if (!_password.empty()) {
			if (!cl.getRequest().count("PASS"))
				return (lp_self(cl, getReplies(cl, 461, "PASS")), false);
			if (cl.getRequest().at("PASS").size() == 0) {
				// lp_self(cl, getReplies(cl, 461, "TOPIC"));
				return false;
			}
			if (cl.getRequest().at("PASS").at(0) != _password) {
				return (lp_self(cl, getReplies(cl, 464)), false);
			
			}
		}
		else if (cl.getRequest().count("PASS"))
			return (lp_self(cl, getReplies(cl, 462, "PASS")), false);
		// NICK CHECK
		if (!cl.getRequest().count("NICK"))
			return (lp_self(cl, getReplies(cl, 431)), false);
		if (cl.getRequest().at("NICK").size() == 0) {
				lp_self(cl, getReplies(cl, 461, "NICK"));
				return false;
		}
		if (cl.getRequest().at("NICK").at(0).size() > 50
			|| cl.getRequest().at("NICK").at(0).find_first_not_of(NICKNAME_VALID_CHAR) != std::string::npos)
			return (lp_self(cl, getReplies(cl, 432, cl.getRequest().at("NICK").at(0))), false);
		for (std::vector<Client *>::iterator itClient = _clients.begin(); itClient != _clients.end(); ++itClient) {
			if ((*itClient)->getStatus() == Client::connected) {
				if ((*itClient)->getRequest().at("NICK").at(0) == cl.getRequest().at("NICK").at(0))
					return (lp_self(cl, getReplies(cl, 433, cl.getRequest().at("NICK").at(0))), false);
			}
		}
		//USER CHECK
		if (!cl.getRequest().count("USER"))
        return (lp_self(cl, getReplies(cl, 461, "USER")), false);
    	// Check if there are exactly 4 arguments for the USER command
		if (cl.getRequest().at("USER").size() != 4)
			return (lp_self(cl, getReplies(cl, 461, "USER")), false);
		// if (cl.getRequest().at("USER").at(0).size() > 50
		// 	|| cl.getRequest().at("USER").at(0).find_first_not_of(NICKNAME_VALID_CHAR) != std::string::npos)
		// 	return (lp_self(cl, getReplies(cl, 432, cl.getRequest().at("USER").at(0))), false);
		// if (cl.getRequest().at("USER").at(1).size() > 50
		// 	|| cl.getRequest().at("USER").at(1).find_first_not_of(NICKNAME_VALID_CHAR) != std::string::npos)
		// 	return (lp_self(cl, getReplies(cl, 432, cl.getRequest().at("USER").at(1))), false);
		// if (cl.getRequest().at("USER").at(2).size() > 50
		// 	|| cl.getRequest().at("USER").at(2).find_first_not_of(USER2_VALID_CHAR) != std::string::npos)
		// 	return (lp_self(cl, getReplies(cl, 432, cl.getRequest().at("USER").at(2))), false);
		return (true);
	}

	bool	Server::connection( Client &cl ) {
		if (cl.getStatus() == Client::connected) {
			return (true);
		}
		// else
		// 	std::cout << ">> NOT connected" << std::endl;
		if (check_connection_error(cl) == false)
			return (false);
		cl.setStatus(Client::connected);
		if (cl.getRequest().at("NICK").at(0) == _admin)
			cl.setRights(Client::operators);
		lp_self(cl, getReplies(cl, 001, cl.getRequest().at("NICK").at(0) + "!" + cl.getRequest().at("USER").at(0) + "@" + cl.getRequest().at("USER").at(2)));
		lp_self(cl, getReplies(cl, 002, cl.getRequest().at("USER").at(2), "10.2"));
		lp_self(cl, getReplies(cl, 003, getCurrentDate()));
		lp_self(cl, getReplies(cl, 004, cl.getRequest().at("USER").at(2), "10.2", "o", "itkol"));
		// for(std::vector<std::string>::const_iterator msg = cl.getMessage().begin(); msg != cl.getMessage().end(); ++msg)
			 // put_line(*msg);
		return (false);
	}

	void Server::createRequest( Client &cl ) {
		static const char *str[] = {"JOIN", "TOPIC", "PRIVMSG", "QUIT", "KICK", "INVITE", "MODE", NULL};
		if (connection(cl)) {
			for (iterator_map it = cl.getRequest().begin(); it != cl.getRequest().end(); ++it) {
				int i;
				for (i = 0; str[i]; ++i)
					if (it->first == str[i])
						break ;
				switch (i) {
					case 0: channelJoin(cl); break ;
					case 1: channelTopic(cl); break ;
					case 2: channelPriv(cl); break ;
                    case 3: channelQuit(cl); break ;
                    case 4: channelKick(cl); break ;
                    case 5: channelInvit(cl); break ;
					case 6: channelMode(cl); break ;
					// case 3: std::cout << str[i] << std::endl; break ;
					// case 4: std::cout << str[i] << std::endl; break ;
					// case 5: std::cout << str[i] << std::endl; break ;
					// case 6: std::cout << str[i] << std::endl; break ;
					default:
						break ;
						// std::cerr << "Invalid request: " << it->first << " - " << it->second << std::endl;
				};
			}
		}
	}


	void Server::addClient(Client * clients){
		_clients.push_back(clients);
	}

	const std::vector<Client *> & Server::getClient(void) const{
		return(_clients);
	}

	const std::string & Server::getPassword(void) const{
		return(_password);
	}

		std::string Server::modeToString(int mode) const {
    switch(mode) {
        case INVITE_ONLY_MODE:
            return "i";
        case TOPIC_RESTRICT_MODE:
            return "t";
        case PASSWORD:
            return "k";
        case LIMIT:
            return "l";
        case PRIVILEGE:
            return "o";
        default:
            return "";
    	}
	}
