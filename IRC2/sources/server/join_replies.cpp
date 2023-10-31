/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join_replies.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ma1iik <ma1iik@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/28 12:14:17 by ahcene            #+#    #+#             */
/*   Updated: 2023/10/31 17:35:22 by ma1iik           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/server/Server.hpp"
#include "../../headers/channel/Channel.hpp"



std::string Server::get_list_connected_users_inchannel(Channel const &channel)
{
	std::string listConnectedUser;
	int size = channel.getConnected().size();
	for (std::set<std::string>::iterator itUser = channel.getConnected().begin(); itUser != channel.getConnected().end(); ++itUser)
	{
			if (channel.isOperator(*itUser) == OPERATOR) // fonction qui determine si un client est opérateur dans un channel donné.
				listConnectedUser += "@";
			listConnectedUser += *itUser;
			size--;
			if (size)
				listConnectedUser += " ";
	}
	std::cout << "listConnectedUser= " << listConnectedUser << std::endl;
	return (listConnectedUser);
}

int Server::user_can_join(Client &client, Channel &toJoin, std::string const &password)
{
	// case : invite-only : si t'es invité tu passes sinon tu passes pas. 
	// case : normal :	si tu as le mot de passe tu passes.
	//					si t'as pas le mot de passe tu passes pas.
	std::string client_name = client.getName();
	if (toJoin.getMode(INVITE_ONLY_MODE, "") == SET)
	{
		if (toJoin.inviteStatus(client_name))
			return (INVITED);
		else
			return (NOT_INVITED);
	}
	else if ((toJoin.getNbr() != 0 && toJoin.getConnectedUserCount() >= toJoin.getNbr())) {
		std::cout << "getNBR: " << toJoin.getNbr() <<std::endl;
		std::cout << "cnctdUsrs: " << toJoin.getConnectedUserCount() <<std::endl;
		return (CHANNEL_FULL);
	}
	else
	{
		if (toJoin.getChannelPass() == password)
			return (GOOD_PASSWORD);
		else
			return (BAD_PASSWORD);
	}
	
	
	
}

void	Server::join_reply(int mode, Client &cl, std::string channel_joined_name)
{
	std::cout <<  "Join reply()" << std::endl;
	if (mode == JOIN_NEW_CHANNEL_OK)
	{
		Channel *channel = getChanByName(channel_joined_name);
		if (channel == NULL)
			perror("Channel not found in vector channel");
		std::string listConnectedUser = get_list_connected_users_inchannel(*channel);
		std::string msg;
		msg = ":" + cl.getRequest().at("NICK").at(0) + " JOIN :" + _channels.back().getName();
		lp_self(cl, msg);
		msg = std::string(":") + SERVER_NAME + " 332 " + " : " +  _channels.back().getName() + " : " + _channels.back().getTopic();
		lp_self(cl, msg);
		msg = std::string(":") + SERVER_NAME + std::string(" 353 ") + cl.getRequest().at("NICK").at(0) + " = " + _channels.back().getName() + " :" + listConnectedUser;
		lp_self(cl, msg);
		msg = std::string(":") + SERVER_NAME + std::string(" 366 ") + cl.getRequest().at("NICK").at(0) + " " + _channels.back().getName() + " :End of NAMES list";
		lp_self(cl, msg);
	}
	if (mode == JOIN_EXISTING_CHANNEL_OK)
	{
		Channel *toJoin = getChanByName(channel_joined_name);
		if (toJoin == NULL)
			perror("Channel not found in vector channel");
		std::string listConnectedUser = get_list_connected_users_inchannel(*toJoin);
		std::string msg;
		msg = ":" + cl.getRequest().at("NICK").at(0) + " JOIN :" + toJoin->getName();
		lp_self(cl, msg);
		lp_channel(cl, *toJoin, msg);
		msg = std::string(":") + SERVER_NAME + " 332 " + " : " +  toJoin->getName() + " : " + _channels.back().getTopic();
		std::cout << "[message topic : " << msg << "]" << std::endl;
		lp_self(cl, msg);
		msg = std::string(":") + SERVER_NAME + " 353 " + cl.getRequest().at("NICK").at(0) + " : " + toJoin->getName() + " :" + listConnectedUser;
		std::cout << "[message user list : " << msg << "]" << std::endl;
		//"<client> <symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}"
		//msg = cl.getName() + "=" + toJoin->getName() + " :" + listConnectedUser + "\r\n";
		lp_self(cl, msg);
		//lp_channel(cl, *toJoin, msg);
		// "<client> <channel> :End of /NAMES list"
		msg = std::string(":") + SERVER_NAME + std::string(" 366 ") + cl.getRequest().at("NICK").at(0) + " " + toJoin->getName() + " :End of NAMES list";
		lp_self(cl, msg);		
		//lp_channel(cl, *toJoin, msg);
	}
	if (mode == BAD_PASSWORD)
	{
		lp_self(cl, getReplies(cl, 475, channel_joined_name));
	}
	if (mode == NOT_INVITED)
	{
		lp_self(cl, getReplies(cl, 473, channel_joined_name)); //indicate JOIN failed cause channel is [invite-only] and client is not invited.
	}
	if (mode == CHANNEL_FULL)
	{
		lp_self(cl, getReplies(cl, 471, channel_joined_name)); //indicate JOIN failed cause channel is [invite-only] and client is not invited.
	}
}