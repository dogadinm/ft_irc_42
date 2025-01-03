/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shovsepy <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/29 03:35:09 by shovsepy          #+#    #+#             */
/*   Updated: 2022/12/29 17:25:31 by shovsepy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/command/Command.hpp"

Pass::Pass(Server* server, bool auth) : Command(server, auth) {}

Pass::~Pass() {}

// syntax: PASS <password>

void    Pass::execute(Client* client, std::vector<std::string> args)
{
    if (args.empty())
    {
        client->reply(ERR_NEEDMOREPARAMS(client->get_nickname(), "PASS"));
        return;
    }

    if (client->is_registered())
    {
        client->reply(ERR_ALREADYREGISTERED(client->get_nickname()));
        return;
    }
    // if (client->get_state() == UNAUTHENTICATED)
    // {
    //     client->reply(ERR_PASSENTERED(client->get_nickname()));
    //     return;   
    // }
    if (_server->get_pass() != args[0].substr(args[0][0] == ':' ? 1 : 0))
    {
        client->reply(ERR_PASSWDMISMATCH(client->get_nickname()));
        return;
    }

    client->set_state(AUTHENTICATED);
}
