# include "../../headers/server/Server.hpp"

	bool	Server::st_isRunning = true;

	bool	Server::start( void ) {
		fd_set		fdwrite[2];
		fd_set		fdread[2];
		int			nfds;
		struct timeval timeout;
		std::string	current_cmd;

		FD_ZERO(&fdwrite[0]);
		FD_ZERO(&fdwrite[1]);
		FD_ZERO(&fdread[0]);
		FD_ZERO(&fdread[1]);


		std::cout << "Socket server: " << _socket << std::endl;
		// int incr=0;
		signal(SIGINT, stop);
		if (errno == EINVAL)
			return (perror("ERROR set SIGINT in function signal: "), false);

		while (Server::st_isRunning) {
			// std::cout << "server tour = " << incr++ << std::endl;
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;
			FD_SET(_socket, &fdread[0]);
			nfds = _socket;
			for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
				if (nfds < (*it)->getSocket())
					nfds = (*it)->getSocket();
			// std::cout << nfds + 1 << std::endl;
			fdread[1] = fdread[0];
			fdwrite[1] = fdwrite[0];
			nfds = select(nfds + 1, &fdread[1], &fdwrite[1], NULL, &timeout);
			// std::cout << "nfds" << nfds << std::endl;
			if (nfds == -1)
				perror("ERROR CAN T FIND ANY EVENT : ");
			// for (int i = 0; i < nfds; ++i) {
				//Est ce qu 'il y a une demande de lecture dans la socket server ?
				// std::cout << "nfds: " << nfds << std::endl;
			if (nfds > 0 && FD_ISSET(_socket, &fdread[1])){
				addClient(new Client(acceptSocket()));
				FD_SET(_clients.back()->getSocket(), &fdread[0]);
				nfds--;
			}
			for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end() && nfds > 0; ++it) 
			{
				if (FD_ISSET((*it)->getSocket(), &fdread[1])) // NICK PASS 
				{
						// std::cout << "FD_ISSET((*it)->getSocket(), &fdread[1])" << std::endl;
				//pour l instant il n y a qu un client mais il faudra faire des boucles.
				// est ce que la "new socket" est en mode lecture pour recevoir des informations du client, il faut  que ce soit en mode read pour utiliser recvSocket.
					const char *str = (*it)->recvSocket();
					nfds--;
					//std::cout << "recv is " << str << std::endl;
					if (str != NULL && strlen(str) && *str != 4 && *str > 0)
					{
						// if (!std::strchr(str, '\n')) {
						// std::cout << "in if" << std::endl;
                        // current_cmd += std::string(str);
						// std::cout << "currend cmd is: " << current_cmd << std::endl << std::endl;
						// }
						// else {
						// 	std::cout << "in else" << std::endl;
						// 	std::string combined_str = current_cmd + std::string(str);
						// 	size_t new_length = combined_str.length();
						// 	char* new_str = new char[new_length + 1];
						// 	std::strcpy(new_str, combined_str.c_str());
						// 	delete[] str; // Free the old str if necessary
						// 	str = new_str;
						// 	std::cout << "final str is: " << str << std::endl << std::endl;
						// }
						if (std::strchr(str, '\n')) {
							// std::cout << "str != NULL && strlen(str) && *str != 4 && *str > 0 /// buf= " << str << std::endl;
							this->getRequest(str, (*it)->setRequest(), *(*it));
							createRequest(*(*it)); // stocker dans le std::vector _message de qui je veux qui recoit le message
							//est ce qu il est possible d ecrire dans la socket sans etre bloqué. on FD SET.
							for(std::set<int>::const_iterator skt = (*it)->getReceivers().begin(); skt != (*it)->getReceivers().end(); ++skt)
							{
								FD_CLR(*skt, &fdread[0]); // mettre dans le boucle de celui a qui je pushback le message. SINON PB !!
								FD_SET(*skt, &fdwrite[0]);
							}
							delete [] str;
							current_cmd = "";
						}
					}
					else
					{
						for (std::vector<Channel>::iterator vit = _channels.begin()
							; vit != _channels.end(); ++vit) {
							for (std::set<std::string>::iterator sit = vit->getConnected().begin()
								; sit != vit->getConnected().end(); ++sit) {
								if (*sit == (*it)->getRequest().at("NICK").at(0)) {
									lp_channel(*(*it), *vit, ":" + (*it)->getRequest().at("NICK").at(0) + " PART " + vit->getName() + " :" + (*it)->getRequest().at("NICK").at(0) + " left the channel.");
									vit->setConnected().erase(sit);
									sit = vit->getConnected().begin();
									if (vit->getConnected().empty())
										break ;
								}
							}
						}
					// 		// le cas ou je ferme definitivement la socket.
						for (std::vector<Client *>::iterator vit = _clients.begin()
						; vit != _clients.end(); ++vit){
							FD_SET((*vit)->getSocket(), &fdwrite[0]);
							FD_CLR((*vit)->getSocket(), &fdread[0]);
							// (*vit)->setMessage("Client disconnected\r\n");
						}
						shutdown((*it)->getSocket(), SHUT_RDWR);
						FD_CLR((*it)->getSocket(), &fdread[0]);
						FD_CLR((*it)->getSocket(), &fdwrite[0]);
						delete *it;
						_clients.erase(it);
						if (str != NULL)
							delete [] str;
						std::cout << "Connexion aborted." << std::endl;
					}	
				}
				else if (FD_ISSET((*it)->getSocket(), &fdwrite[1])){
					std::string fullmessage;
					for(std::vector<std::string>::const_iterator msg = (*it)->getMessage().begin(); msg != (*it)->getMessage().end(); ++msg)
						fullmessage += *msg;
					(*it)->clearMessage();
					(*it)->sendSocket(fullmessage.c_str(), fullmessage.size(), 0);
					nfds--;
					FD_CLR((*it)->getSocket(), &fdwrite[0]);
					FD_SET((*it)->getSocket(), &fdread[0]);
				}
			}
		}
		signal(SIGINT, SIG_IGN);
		if (errno == EINVAL)
			return (perror("ERROR set SIGIGN in function signal: "), false);
		return (true);
	}

	void	Server::stop( int signum ) { 
		(void)signum;
		Server::st_isRunning = false;
		std::cout << "\rServer is down" << std::endl;
	}
