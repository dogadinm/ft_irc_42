/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp	                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shovsepy <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/25 15:34:45 by shovsepy          #+#    #+#             */
/*   Updated: 2022/12/25 16:36:09 by shovsepy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_CLIENT_HPP
#define IRC_CLIENT_HPP

#include <sys/socket.h>
#include <sys/poll.h>

#include <string>
#include <vector>
#include <ctime>

class Client;

#include "Channel.hpp"
#include "../log.hpp"


enum ClientState
{
    // PASS,
    UNAUTHENTICATED,
    AUTHENTICATED,
    REGISTERED,
    DISCONNECTED
};


class Client 
{
    typedef std::vector<Channel *>::iterator channel_iterator;
    private:
        
        int             _fd;
        std::string     _port;
        bool            _admin_access;

        std::string     _nickname;
        std::string     _username;
        std::string     _realname;
        std::string     _hostname;

        ClientState     _state;
        std::vector<Channel *>  _channels;
        time_t  lastActivityTime;

        Client();
        Client(const Client &src);

    public:

        /* Costructor and Destructor */
        
        Client(int fd, const std::string &port, const std::string &hostname);
        ~Client();


        /* Getters */

        int             get_fd() const;
        bool            get_admin_access() const;
        std::string     get_port() const;

        std::string     get_nickname() const;
        std::string     get_username() const;
        std::string     get_realname() const;
        std::string     get_hostname() const;
        std::string     get_prefix() const;
        ClientState     get_state() const;
        std::vector<Channel *> get_channels() const;
        Channel*        get_channel(std::string name) const;

    
        /* Setters */

        void            set_nickname(const std::string &nickname);
        void            set_username(const std::string &username);
        void            set_realname(const std::string &realname);
        void            set_state(ClientState state);
        void            set_channel(Channel *channel);
        void            set_admin_access(bool status);

        /* Check state */

        bool            is_registered() const;


        /* Send/Recieve Actions */

        void            write(const std::string& message) const;
        void            reply(const std::string& reply);

        void            welcome();


        /* Client Actions */

        void            join(Channel *channel);
        void            leave(Channel* channel);
        void            leave_all_channels();

        void            remove_channel(Channel* channel);
        int             get_channel_count() const;
        double          get_idle_time() const;
        void            update_activity();
};

#endif
