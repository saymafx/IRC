

# include "../../headers/server/Server.hpp"

Client *Server::findReceiver(std::string clientName)
{
	Client *client;
	client = NULL;
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it){
		if ((*it)->getStatus() == Client::disconnected)
			continue;
		if ((*it)->getRequest().count("NICK") && ((*it)->getRequest().at("NICK").at(0) == clientName)){
			client = *it;
			return (client);
		}
	}
	std::cout << "in FINDRECEIVER INVALID CLIENT NAME RECEIVER" << std::endl;
	return (client);
}
	void Server::lp_self(Client &selfClient, std::string message){
		selfClient.setReceivers(selfClient.getSocket());
		selfClient.setMessage(message + "\n");

	}
	
	void Server::lp_channel(Client &currentClient, Channel &currentChannel, std::string message){

		for (std::set<std::string>::iterator itChan = currentChannel.getConnected().begin(); itChan != currentChannel.getConnected().end(); ++itChan)
		{
			if (currentClient.getRequest().count("NICK") && currentClient.getRequest().at("NICK").at(0) == *itChan)
				continue;
			// SOCKET DES CONNECTED USERS INSIDE CHSNNEL
			Client *clientReceiver = findReceiver(*itChan);
			currentClient.setReceivers(clientReceiver->getSocket());
			clientReceiver->setMessage(message + "\n");
		}
	}
	
	void Server::lp_server(Client &currentClient, std::string message){
		if (currentClient.getStatus() == Client::disconnected)
			return;
		for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it){
			if ((*it)->getStatus() == Client::disconnected)
				continue;
			if ((*it)->getRequest().count("NICK") && ((*it)->getRequest().at("NICK").at(0) != currentClient.getRequest().at("NICK").at(0))){
				(*it)->setMessage(message + "\n");
				currentClient.setReceivers((*it)->getSocket());
			}	
		}
	}
	
	void Server::lp_private(Client &currentClient, std::string clientName, std::string message){
		

		Client *clientReceiver = findReceiver(clientName);
		currentClient.setReceivers(clientReceiver->getSocket());
		clientReceiver->setMessage(message + "\n");
	}
	