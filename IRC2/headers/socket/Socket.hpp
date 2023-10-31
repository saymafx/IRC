#ifndef __SOCKET_HPP__
# define __SOCKET_HPP__

# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <cstdio>
# include <iostream>
# include <unistd.h>
# include <cstring>
// only for mac - -- -- - - - -
#include <fcntl.h>

#define MACOS 1
// --- - - -- - -- -- - - - - - - - -- 
# ifndef MAX_LISTEN
#  ifdef SOMAXCONN
#   define MAX_LISTEN SOMAXCONN
#  else
#	define MAX_LISTEN 128
#  endif
# endif

class Socket
{	
protected:
	const int _socket;
	struct sockaddr_in _addr;
	socklen_t _sizeAddr;

	Socket(const int socket);
	Socket(const int domain, const int type, const int protocol);

public:
	virtual	~Socket();

	bool	setSocket(const short family, const char *addr, const unsigned short port);
	bool    setSockOpt( const int, const int, const void *, socklen_t );

	int 	getSocket(void);
	bool	bindSocket(void) const;
	bool	listenSocket(const int backlog) const;
	int		acceptSocket(void) const;
	bool 	sendSocket(const char *buf, const size_t len, const int flags) const;
	char	*recvSocket(void) const;


};

#endif
