# include "../headers/main.hpp"

int	main( int argc, char **argv )
{
	if (argc != 2 && argc != 3) { // a ajuster avant soutenance
		std::cerr << "Invalid arguments" << std::endl;
		return (1);
	}

	
	int port;

	std::istringstream	ss(argv[1]);
	ss >> port;
	if (port < 0 || port > 65535) { // a ajuster avant soutenance
		std::cerr << "Invalid port" << std::endl;
		return (1);
	}
	std::cout << "Let's go !" << std::endl;
	std::string password;
	if (argc == 3)
		password = argv[2];
	else if (argc == 2)
		password = "";
	Server s1("127.0.0.1", port, password); // 0.0.0.0 :   ecoute sur toutes les ip de ce reseau local // sur ce reseau y a que ahcene: 127..1

	s1.start();

	return (0);
}
