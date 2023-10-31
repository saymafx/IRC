#ifndef __EVENTS_HPP__
# define __EVENTS_HPP__
	
	void	put_line( std::string line ) {
		for (std::string::iterator it = line.begin();
			it != line.end(); ++it) {
			if (*it==13)
				std::cout << "(\\r)" << std::flush;
			else if (*it==10)
				std::cout << "(\\n)" << std::endl;
			else if (*it > 31 && *it < 127)
				std::cout << *it << std::flush;
			else
				std::cout << (int)*it << std::flush;
		}
		return ;
	}

	bool	start( void );

	static void	stop( int signum );

#endif
