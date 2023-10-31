# include "../../headers/socket/Socket.hpp"
# include <stdlib.h>


	Socket::Socket(const int socket) : _socket(socket), _addr(), _sizeAddr(0)  {
		if (_socket == -1){
			perror("ERROR SOCKET INITIALIZATION: ");
			exit(1);
		}
	}

	Socket::Socket(const int domain, const int type, const int protocol)
		: _socket(socket(domain, type, protocol)), _addr(), _sizeAddr(0)	{
		if (_socket == -1){
			perror("ERROR SOCKET INITIALIZATION: ");
			exit(1);
		}
	}

	Socket::~Socket(){
		if (_socket != -1 && close(_socket) == -1){
			perror("ERROR CLOSING SOCKET: ");
			exit(1);
		}
	}

	int Socket::getSocket(void){ return (_socket);}

	bool	Socket::setSocket(const short family, const char *addr, const unsigned short port){
		_addr.sin_family = family;
		_addr.sin_port = htons(port);
		_addr.sin_addr.s_addr = inet_addr(addr);
		# ifdef __LINUX__
			if (_addr.sin_addr.s_addr == inet_addr("-1")){ // 255.255.255.255
				perror("ERROR SETTING SOCKET");
				exit(1);
				return (false);
			}
		# endif
		std::memset(_addr.sin_zero, 0, 8);
		_sizeAddr = sizeof(_addr);
		return (true);
	}

	bool    Socket::setSockOpt( const int level, const int option_name, const void *option_value, socklen_t option_len){
    	return (setsockopt(_socket, level, option_name, option_value, option_len) != -1 ? true : (perror("ERROR setSockOpt: "), false));
	}

	bool	Socket::bindSocket(void) const{
		if (bind(_socket, (const struct sockaddr *)&_addr, _sizeAddr) == -1){
			perror("ERROR BINDING SOCKET: ");
			exit(1);
			return(false);
		}
		return (true);
	}

	bool	Socket::listenSocket(const int backlog) const{
		if (listen(_socket, backlog) == -1)
			return(perror("ERROR LISTENING SOCKET: "), false);
		return (true);
	}

	int		Socket::acceptSocket(void) const{
		int newsocket = accept(_socket, (struct sockaddr*)&_addr, (socklen_t*)&_sizeAddr);
		if (newsocket == -1)
			return(perror("ERROR ACCEPTING SOCKET: "), -1);
		return (newsocket);
	}

	bool 	Socket::sendSocket(const char *buf, const size_t len, const int flags) const{
		if (send(_socket, buf, len, flags) == -1)
			return(perror("ERROR CAN T SEND VIA SOCKET: "), false);
		return(true);
	}

	char	*Socket::recvSocket(void) const{
		#ifdef __APPLE__
			fcntl(_socket, F_SETFL, O_NONBLOCK);
    	#endif
		char	*buf = new char[1024];
		if (buf == NULL)
			return(std::cerr << "ERROR WRITE BUF" << std::endl, (char *)NULL);
		std::string	tmp;
		while (1) {
			int ret = recv(_socket, buf, 1024, 0);
		#ifdef __APPLE__
		if (ret <= 0) {
				if (ret == 0)
				{
					delete[] buf;
					return(perror("ERROR CLIENT IS DISCONNECTED: "), (char *)NULL);
				}
				if (ret == -1)
					break;
			}
    	#endif
		#ifdef __LINUX__
			if (ret < 1) {
				delete[] buf;
				if (ret == 0)
					return(perror("ERROR CLIENT IS DISCONNECTED: "), (char *)NULL);
				return(perror("ERROR CAN T RECEIVE VIA SOCKET: "), (char *)NULL);
			}
		#endif
			buf[ret] = '\0';
			tmp += buf;
			// std::cout << "tmp --->" << tmp << std::endl;
			if (tmp.find("\r\n") != std::string::npos || tmp.find("\n") != std::string::npos)
				break ;
		}
		
		delete[] buf;
		buf = new char[tmp.size() + 1];
		// 	tmp.insert(tmp.end() - 1, '\r');
		tmp.copy(buf, tmp.size());
		buf[tmp.size()] = '\0';
		// std::cout << "buf final --->" << buf << std::endl;
		return (buf); 
	}
