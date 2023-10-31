#ifndef __SERVER_HPP__
# define __SERVER_HPP__


// create a socket - Get the file descriptor!
// bind to an address -What port am I on?
// listen on a port, and wait for a connection to be established.
// accept the connection from a client.
// send/recv - the same way we read and write for a file.
// shutdown to end read/write.
// close to releases data.
# include <string>
# include <vector>
# include <cerrno>
# include <signal.h>
# include <iostream>
# include <sys/time.h>
# include <sys/select.h>
# include <cstdio>
# include <iterator>
# include <ctime>
# include <algorithm>


# include "../socket/Socket.hpp"
# include "../client/Client.hpp"

# define NICKNAME_VALID_CHAR "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_[]{}\\`|"
# define USER2_VALID_CHAR "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.[]{}\\`|"

# ifndef SERVER_NAME
#  define SERVER_NAME "127.0.0.1"
# endif



	// typedef enum srv_error {

	// 	ERR_NEEDMOREPARAMS,
	// 	ERR_BANNEDFROMCHAN,
	// 	ERR_INVITEONLYCHAN,
	// 	ERR_BADCHANNELKEY,
	// 	ERR_CHANNELISFULL,
	// 	ERR_BADCHANMASK,
	// 	ERR_NOSUCHCHANNEL,
	// 	ERR_TOOMANYCHANNELS,
	// 	ERR_TOOMANYTARGETS,
	// 	ERR_UNAVAILRESOURCE,
	// 	ERR_NOTONCHANNEL,
	// 	ERR_CHANOPRIVSNEEDED,
	// 	ERR_NOCHANMODES,
	// 	ERR_USERNOTINCHANNEL,
	// 	ERR_NOSUCHNICK,
	// 	ERR_USERONCHANNEL,
	// 	ERR_KEYSET,
	// 	ERR_UNKNOWNMODE,
	// 	RPL_NOTOPIC,
	// 	RPL_INVITING,
	// 	RPL_AWAY,
	// 	RPL_CHANNELMODEIS,
	// 	RPL_BANLIST,
	// 	RPL_ENDOFBANLIST,
	// 	RPL_EXCEPTLIST,
	// 	RPL_ENDOFEXCEPTLIST,
	// 	RPL_INVITELIST,
	// 	RPL_ENDOFINVITELIST,
	// 	RPL_UNIQOPIS,
	// 	RPL_TOPIC,
	// 	OK,

	// }src_errors;


class Server : public Socket
{
public:
	typedef std::map<std::string, std::vector<std::string> >::const_iterator	iterator_map;

private:

	static bool				st_isRunning;

	std::string				_addr;
	std::string				_password;
	unsigned short			_port;
	std::vector<Client *>	_clients;
	std::vector<Channel> 	_channels;
	std::string				_admin;


public:
	Server(const std::string & addr, const unsigned short port, const std::string password = "");
	~Server();

	std::vector<std::string>	split(std::string line, std::string delimiteur);
	std::string getCurrentDate();
	void	getRequest(const char * str, std::map<std::string, std::vector<std::string> > &request, const Client &cl);
	bool	check_connection_error( Client &cl );
	bool	connection( Client &cl );
	void	createRequest(Client &cl);


	Client *getClientByName(const std::string &nameOfClient);
	void 	execTopic(Client &cl, std::string channel, std::string newtopic);

	void	quitChan(Client &cl, Channel &chan);
	void	execJoin(Client &cl, std::vector<std::string> &listChannel, std::vector<std::string> &listPassword);
	void	parsJoin(std::vector<std::string> & listChannel, std::vector<std::string> & listPassword, std::vector<std::string> & listMode, std::vector<std::string>  req);
	// parse join utils
	void	caseSeparator(std::string &channelName, std::string &mode, size_t poz, size_t poz_end);
	void	caseNoseparator(std::string &channelName, std::string &mode);
	std::string stringTolower(std::string &str);
	bool	checkChanError(Client &cl, std::vector<std::string> &listChannel);
	bool	checkModError(Client &cl, std::vector<std::string> &listMode);
	int		checkGlobalError(std::vector<std::string> &listChannel, std::vector<std::string> &listMode, std::vector<std::string> & listPassword);
	std::string getReplies(Client &cl, unsigned short msgError, std::string arg1 = "", std::string arg2 = "", std::string arg3 = "", std::string arg4 = "", std::string arg5 = "", std::string arg6 = "", std::string arg7 = "");
	void parsChan2(std::vector<std::string> & subline, std::string req);

	void channelQuit(Client &cl);
    void channelKick(Client &cl);
	void channelJoin(Client &cl);
	void channelTopic(Client &cl);
	void channelPriv(Client &cl);
	void channelInvit(Client &cl);
	void channelMode(Client &cl);

	void execModes(Client &cl, Channel &chan);
	bool CharOutOfRange(char c);
	int  findOpt(char c);

	bool channelExists(const std::string& channelName) const;

	// struct ChannelComparator {
	// 	std::string targetName;
	// 	ChannelComparator(const std::string& name) : targetName(name) {}
	// 	bool operator()(const Channel& channel) const {
	// 		return channel.getName() == targetName;
	// 	}
	// };

	void parsChanPriv(std::vector<std::string> & subline, std::string req);
	void addClient(Client *);
	bool checkExistingChan(std::string chName);

	// loop utils
	Client *findReceiver(std::string clientName);
	// loop funcitons
	void lp_self(Client &selfClient, std::string message);
	void lp_channel(Client &currentClient, Channel &currentChannel, std::string message);
	void lp_server(Client &cl, std::string message);
	void lp_private(Client &currentClient, std::string clientName, std::string message);

	const std::string & getPassword(void) const;
	const std::vector<Client *> & getClient(void) const ;
	// JOIN FUNCTIONS
	void	join_reply(int mode, Client &cl, std::string channel_joined_name);

	void	join_existing_channel(Channel *channel_tojoin, Client &client);
	void	create_new_channel(std::string const & channel_name, std::string const &channel_pass, Client &client);
	Channel *getChanByName(const std::string &nameOfChannel);
	std::map<std::string, std::string> fill_map(std::vector<std::string> listChannel, std::vector<std::string> listPassword); 
	int 	user_can_join(Client &client, Channel &toJoin, std::string const &password);
std::string get_list_connected_users_inchannel(Channel const &channel);

	std::string		modeToString(int mode) const;
	bool 			execTopicAllowed(Client &cl, std::string channel);
	std::string		trimEndNewline(const std::string& s);
	void 			channelNick(Client &cl);


	/////replies
	/******************************************************************************/
/*                                   0**                                      */
/******************************************************************************/
std::string RPL_WELCOME(std::string prefix) { return ":Welcome to the Internet Relay Network " + prefix; }
std::string RPL_YOURHOST(std::string servername, std::string ver) { return ":Your host is " + servername + ", running version " + ver; }
std::string RPL_CREATED(std::string date) { return ":This server was created " + date; }
std::string RPL_MYINFO(std::string servername, std::string version, std::string umodes, std::string cmodes) { return servername + " " + version + " " + umodes + " " + cmodes; }
std::string RPL_BOUNCE(std::string server, std::string port) { return ":Try server " + server + ", port " + port; }
/******************************************************************************/
/*                                   2**                                      */
/******************************************************************************/
std::string RPL_TRACELINK(std::string version, std::string destination, std::string server) { return "Link " + version + " " + destination + " " + server; }
std::string RPL_TRACECONNECTING(std::string class1, std::string server) { return "Try. " + class1 + " " + server; }
std::string RPL_TRACEHANDSHAKE(std::string class1, std::string server) { return "H.S. " + class1 + " " + server; }
std::string RPL_TRACEUNKNOWN(std::string class1, std::string ip) { return "???? " + class1 + " " + ip; }
std::string RPL_TRACEOPERATOR(std::string class1, std::string nick) { return "Oper " + class1 + " " + nick; }
std::string RPL_TRACEUSER(std::string class1, std::string nick) { return "User " + class1 + " " + nick; }
std::string RPL_TRACESERVER(std::string class1, std::string int1, std::string int2, std::string server, std::string nick, std::string user, std::string host) { return "Serv " + class1 + " " + int1 + "S " + int2 + "C " + server + " " + nick + "!" + user + "@ " + host; }
std::string RPL_TRACENEWTYPE(std::string type, std::string name) { return type + " 0 " + name; }
std::string RPL_STATSLINKINFO(std::string name, std::string q, std::string smessages, std::string sbytes, std::string rsmessages, std::string rbytes, std::string time) { return name + " " + q + " " + smessages + " " + sbytes + " " + rsmessages + " " + rbytes + " " + time; }
std::string RPL_STATSCOMMANDS(std::string command, std::string count) { return command + " " + count; }
std::string RPL_STATSCLINE(std::string host, std::string name, std::string port, std::string class1) { return "C " + host + " * " + name + " " + port + " " + class1; }
std::string RPL_STATSNLINE(std::string host, std::string name, std::string port, std::string class1) { return "N " + host + " * " + name + " " + port + " " + class1; }
std::string RPL_STATSILINE(std::string host1, std::string host2, std::string port, std::string class1) { return "I " + host1 + " * " + host2 + " " + port + " " + class1; }
std::string RPL_STATSKLINE(std::string host, std::string user, std::string port, std::string class1) { return "K " + host + " * " + user + " " + port + " " + class1; }
std::string RPL_STATSYLINE(std::string class1, std::string ping, std::string connect, std::string max) { return "Y " + class1 + " " + ping + " " + connect + " " + max; }
std::string RPL_ENDOFSTATS(std::string letter) { return letter + " :End of /STATS report"; }
std::string RPL_UMODEIS(std::string mode) { return mode; }
std::string RPL_STATSLLINE(std::string host, std::string name, std::string depth) { return "L " + host + " * " + name + " " + depth; }
std::string RPL_STATSUPTIME() { return ":Server Up %d days %d:%02d:%02d"; }
std::string RPL_STATSOLINE(std::string mask, std::string name) { return "O " + mask + " * " + name; }
std::string RPL_STATSHLINE(std::string mask, std::string name) { return "H " + mask + " * " + name; }
std::string RPL_LUSERCLIENT(std::string int1, std::string int2, std::string int3) { return ":There are " + int1 + " users and " + int2 + " invisible on " + int3 + " servers"; }
std::string RPL_LUSEROP(std::string int1) { return int1 + " :operator(s) online"; }
std::string RPL_LUSERUNKNOWN(std::string int1) { return int1 + " :unknown connection(s)"; }
std::string RPL_LUSERCHANNELS(std::string int1) { return int1 + " :channels formed"; }
std::string RPL_LUSERME(std::string int1, std::string int2) { return ":I have " + int1 + " clients and " + int2 + " servers"; }
std::string RPL_ADMINME(std::string server) { return server + " :Administrative info"; }
std::string RPL_ADMINLOC1(std::string info) { return ":" + info; }
std::string RPL_ADMINLOC2(std::string info) { return ":" + info; }
std::string RPL_ADMINEMAIL(std::string info) { return ":" + info; }
std::string RPL_TRACELOG(std::string file, std::string level) { return "File " + file + " " + level; }
std::string RPL_TRACEEND(std::string server, std::string version) { return server + " " + version + " :End of TRACE"; }
std::string RPL_TRYAGAIN(std::string cmd) { return cmd + " :Please wait a while and try again."; }
/******************************************************************************/
/*                                   3**                                      */
/******************************************************************************/
std::string RPL_NONE() { return ""; }
std::string RPL_AWAY(std::string nick, std::string message) { return nick + " :" + message; }
std::string RPL_USERHOST(std::string reply_list) { return ":" + reply_list; }
std::string RPL_ISON(std::string nick_list) { return ":" + nick_list; }
std::string RPL_UNAWAY() { return ":You are no longer marked as being away"; }
std::string RPL_NOWAWAY() { return ":You have been marked as being away"; }
std::string RPL_WHOISUSER(std::string nick, std::string user, std::string host, std::string real) { return nick + " " + user + " " + host + " * :" + real; }
std::string RPL_WHOISSERVER(std::string nick, std::string server, std::string info) { return nick + " " + server + " :" + info; }
std::string RPL_WHOISOPERATOR(std::string nick) { return nick + " :is an IRC operator"; }
std::string RPL_WHOWASUSER(std::string nick, std::string user, std::string host, std::string real) { return nick + " " + user + " " + host + " * :" + real; }
std::string RPL_ENDOFWHO(std::string name) { return name + " :End of /WHO list"; }
std::string RPL_WHOISIDLE(std::string nick, std::string integer) { return nick + " " + integer + " :seconds idle"; }
std::string RPL_ENDOFWHOIS(std::string nick) { return nick + " :End of /WHOIS list"; }
std::string RPL_WHOISCHANNELS(std::string nick, std::string channel) { return nick + " :" + channel; }
std::string RPL_LISTSTART() { return "Channel :Users  Name"; }
std::string RPL_LIST(std::string channel, std::string nbrUser, std::string topic) { return channel + " " + nbrUser + " :" + topic; }
std::string RPL_LISTEND() { return ":End of /LIST"; }
std::string RPL_CHANNELMODEIS(std::string channel, std::string mode, std::string params) { return channel + " " + mode + " " + params; }
std::string RPL_UNIQOPIS(std::string channel, std::string nick) { return channel + " " + nick; }
std::string RPL_NOTOPIC(std::string channel) { return channel + " :No topic is set"; }
std::string RPL_TOPIC(std::string channel, std::string topic) { return channel + " :" + topic; }
std::string RPL_INVITING(std::string channel, std::string nick) { return nick + " " + channel; }
std::string RPL_SUMMONING(std::string user) { return user + " :Summoning user to IRC"; }
std::string RPL_INVITELIST(std::string channel, std::string inviter) { return channel + " " + inviter; }
std::string RPL_ENDOFINVITELIST(std::string channel) { return channel + " :End of channel invite list"; }
std::string RPL_EXCEPTLIST(std::string channel, std::string mask) { return channel + " " + mask; }
std::string RPL_ENDOFEXCEPTLIST(std::string channel) { return channel + " :End of channel exception list"; }
std::string RPL_VERSION(std::string version, std::string server, std::string comments) { return version + " " + server + " :" + comments; }
std::string RPL_WHOREPLY(std::string channel, std::string user, std::string host, std::string server, std::string nick, std::string state, std::string real) { return channel + " " + user + " " + host + " " + server + " " + nick + " " + state + " :0 " + real; }
std::string RPL_NAMREPLY(std::string chan_mod, std::string channel, std::string nick_list) { return chan_mod + " " + channel + " :" + nick_list; }
std::string RPL_LINKS(std::string mask, std::string server, std::string hopcount, std::string info) { return mask + " " + server + " :" + hopcount + " " + info; }
std::string RPL_ENDOFLINKS(std::string mask) { return mask + " :End of /LINKS list"; }
std::string RPL_ENDOFNAMES(std::string channel) { return channel + " :End of /NAMES list"; }
std::string RPL_BANLIST(std::string channel, std::string banip) { return channel + " " + banip; }
std::string RPL_ENDOFBANLIST(std::string channel) { return channel + " :End of channel ban list"; }
std::string RPL_ENDOFWHOWAS(std::string nick) { return nick + " :End of WHOWAS"; }
std::string RPL_INFO(std::string string) { return ":" + string; }
std::string RPL_MOTD(std::string text) { return ":- " + text; }
std::string RPL_ENDOFINFO() { return ":End of /INFO list"; }
std::string RPL_MOTDSTART(std::string server) { return ":- " + server + " Message of the day - "; }
std::string RPL_ENDOFMOTD() { return ":End of /MOTD command"; }
std::string RPL_YOUREOPER() { return ":You are now an IRC operator"; }
std::string RPL_REHASHING(std::string file) { return file + " :Rehashing"; }
std::string RPL_YOURESERVICE(std::string nick) { return ":You are service " + nick; }
std::string RPL_TIME(std::string server, std::string time) { return server + " :" + time; }
std::string RPL_USERSSTART() { return ":UserID Terminal Host"; }
std::string RPL_USERS() { return ":%-8s %-9s %-8s"; }
std::string RPL_ENDOFUSERS() { return ":End of users"; }
std::string RPL_NOUSERS() { return ":Nobody logged in"; }
/******************************************************************************/
/*                                   4**                                      */
/******************************************************************************/
std::string ERR_NOSUCHNICK(std::string nickname) { return nickname + " :No such nick/channel"; }
std::string ERR_NOSUCHSERVER(std::string server) { return server + " :No such server"; }
std::string ERR_NOSUCHCHANNEL(std::string channel) { return channel + " :No such channel"; }
std::string ERR_CANNOTSENDTOCHAN(std::string channel) { return channel + " :Cannot send to channel"; }
std::string ERR_TOOMANYCHANNELS(std::string channel) { return channel + " :You have joined too many channels"; }
std::string ERR_WASNOSUCHNICK(std::string nickname) { return nickname + " :There was no such nickname"; }
std::string ERR_TOOMANYTARGETS(std::string target) { return target + " :Duplicate recipients. No message delivered"; }
std::string ERR_NOSUCHSERVICE(std::string service) { return service + " :No such service"; }
std::string ERR_NOORIGIN() { return ":No origin specified"; }
std::string ERR_NORECIPIENT(std::string command) { return ":No recipient given (" + command + ")"; }
std::string ERR_NOTEXTTOSEND() { return ":No text to send"; }
std::string ERR_NOTOPLEVEL(std::string mask) { return mask + " :No toplevel domain specified"; }
std::string ERR_WILDTOPLEVEL(std::string mask) { return mask + " :Wildcard in toplevel domain"; }
std::string ERR_BADMASK(std::string mask) { return mask + " :Bad Server/host mask"; }
std::string ERR_UNKNOWNCOMMAND(std::string command) { return command + " :Unknown command"; }
std::string ERR_NOMOTD() { return ":MOTD File is missing"; }
std::string ERR_NOADMININFO(std::string server) { return server + " :No administrative info available"; }
std::string ERR_FILEERROR(std::string op, std::string file) { return ":File error doing " + op + " on " + file; }
std::string ERR_NONICKNAMEGIVEN() { return ":No nickname given"; }
std::string ERR_ERRONEUSNICKNAME(std::string nick) { return nick + " :Erroneus nickname"; }
std::string ERR_NICKNAMEINUSE(std::string nick) { return nick + " :Nickname is already in use"; }
std::string ERR_NICKCOLLISION(std::string nick) { return nick + " :Nickname collision KILL"; }
std::string ERR_USERNOTINCHANNEL(std::string nick, std::string channel) { return nick + " " + channel + " :They aren't on that channel"; }
std::string ERR_NOTONCHANNEL(std::string channel) { return channel + " :You're not on that channel"; }
std::string ERR_USERONCHANNEL(std::string user, std::string channel) { return user + " " + channel + " :is already on channel"; }
std::string ERR_NOLOGIN(std::string user) { return user + " :User not logged in"; }
std::string ERR_SUMMONDISABLED() { return ":SUMMON has been disabled"; }
std::string ERR_USERSDISABLED() { return ":USERS has been disabled"; }
std::string ERR_NOTREGISTERED() { return ":You have not registered"; }
std::string ERR_NEEDMOREPARAMS(std::string command) { return command + " :Not enough parameters"; }
std::string ERR_ALREADYREGISTRED() { return ":You may not reregister"; }
std::string ERR_NOPERMFORHOST() { return ":Your host isn't among the privileged"; }
std::string ERR_PASSWDMISMATCH() { return ":Password incorrect"; }
std::string ERR_YOUREBANNEDCREEP() { return ":You are banned from this server"; }
std::string ERR_KEYSET(std::string channel) { return channel + " :Channel key already set"; }
std::string ERR_CHANNELISFULL(std::string channel) { return channel + " :Cannot join channel (+l)"; }
std::string ERR_UNKNOWNMODE(std::string c) { return c + " :is unknown mode char to me"; }
std::string ERR_INVITEONLYCHAN(std::string channel) { return channel + " :Cannot join channel (+i)"; }
std::string ERR_BANNEDFROMCHAN(std::string channel) { return channel + " :Cannot join channel (+b)"; }
std::string ERR_BADCHANNELKEY(std::string channel) { return channel + " :Cannot join channel (+k)"; }
std::string ERR_BADCHANMASK(std::string channel) { return channel + " :Bad Channel Mask"; }
std::string ERR_NOCHANMODES(std::string channel) { return channel + " :Channel doesn't support modes"; }
std::string ERR_BANLISTFULL(std::string channel) { return channel + " :Channel list is full"; }
std::string ERR_NOPRIVILEGES() { return ":Permission Denied- You're not an IRC operator"; }
std::string ERR_CANTKILLSERVER() { return ":You cant kill a server!"; }
std::string ERR_RESTRICTED() { return ":Your connection is restricted!"; }
std::string ERR_CHANOPRIVSNEEDED(std::string channel) { return channel + " :You're not channel operator"; }
std::string ERR_UNIQOPPRIVSNEEDED() { return ":You're not the original channel operator"; }
std::string ERR_NOOPERHOST() { return ":No O-lines for your host"; }
std::string ERR_UMODEUNKNOWNFLAG() { return ":Unknown MODE flag"; }
std::string ERR_USERSDONTMATCH() { return ":Cant change mode for other users"; }


	# include "../events/Events.hpp"

};

#endif
