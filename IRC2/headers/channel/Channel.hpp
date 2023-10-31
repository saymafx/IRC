#ifndef __CHANNEL_HPP__
# define __CHANNEL_HPP__

# include <string>
# include <set>
# include <map>
# include <list>


#define INVITE_ONLY '#'
#define SECRET '+'
#define STANDARD '!'
#define unknown '&'

	typedef enum _profile {
		// following enums describing user in channel modes
		JOIN_NEW_CHANNEL_OK,
		JOIN_EXISTING_CHANNEL_OK,
	    REGULAR,
	    OPERATOR,
		INVITED,
		GOOD_PASSWORD,
		BAD_PASSWORD,
		NOT_INVITED,
		CHANNEL_FULL,
		QUIT_ALL_CHANNELS,
		// following enums describing channel modes
		SET,
		UNSET,
		INVITE_ONLY_MODE,
		TOPIC_RESTRICT_MODE,
		PASSWORD,
		LIMIT,
		PRIVILEGE,	
	}profile;

// · i: Set/remove Invite-only channel
// · t: Set/remove the restrictions of the TOPIC command to channel operators
// · k: Set/remove the channel key (password)
// · o: Give/take channel operator privilege
// · l: Set/remove the user limit to channel
typedef struct _chanmode
{
	int invite_only;
	int topic_restriction;
	int channel_key;


}_chanmode;

class Channel
{
public:
	typedef enum ch_rights{
		operators,
		regular_user,
	}ch_rights;


private:
	const std::string					_channelName;

	size_t 								_nbrClients; // limit set >0  unset 0
	std::string							_channelPassword;
	int 								invite_only;
	int 								topic_restriction;

	std::set<std::string>				_connectedUsers;
	std::map<std::string, ch_rights>	_userList;
	std::list<std::string>              _adminList;
	std::string							_topic;
	std::set<std::string>				_invitedUsers;
	//ch_mod 								_rights;

public:
	Channel(const std::string & channelname, const std::string password);
	~Channel();

	int 							setMode(int mode, int action, int limit, std::string arg, std::string chan, std::string cl);
	int 									getMode(int mode, std::string arg); 

	void 									setChannelPass(std::string channelPass);
	std::set<std::string> 					&setConnected(void);
	std::map<std::string, ch_rights> 		&setList(void);
	void									addConnectedUser(std::string username);
	
	void									setNbr(size_t nbrClients);
	void 									setTopic(std::string topic);
	void 									setInvite(std::string user);
	bool 									inviteStatus(std::string username);
	const std::string 						&getName(void) const;
	const std::string 						&getPassword(void) const;
	const std::set<std::string> 			&getConnected(void) const;
	const std::map<std::string, ch_rights> 	&getList(void) const;
	const std::set<std::string> 			&getInvitedUsers(void) const;
	//const Channel::ch_mod 					&getRights(void);
	size_t 									getNbr(void) const;
	std::string 							getTopic(void) const;
	void 									setChanAdmin(std::string username);
    const std::list<std::string> 			&getChanAdmin(void) const;
    const std::string 						&getChannelPass(void) const;
    int	 									isOperator(std::string User) const;
    int	 									isInvited(std::string User) const;
    bool									isConnected(std::string user) const;

	size_t 									getConnectedUserCount() const;

};


#endif