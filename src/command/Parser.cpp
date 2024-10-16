#include "../../include/command/Parser.hpp"


Parser::Parser(Server* server): _server(server)
{
    _commands["PASS"] = new Pass(_server, false);
    _commands["NICK"] = new Nick(_server, false);
    _commands["USER"] = new User(_server, false);
    _commands["QUIT"] = new Quit(_server, false);

    // _commands["PING"] = new Ping(_srv);
    // _commands["PONG"] = new Pong(_srv);
    // _commands["JOIN"] = new Join(_srv);
    // _commands["PART"] = new Part(_srv);
    // _commands["KICK"] = new Kick(_srv);
    // _commands["MODE"] = new Mode(_srv);

	// _commands["PRIVMSG"] = new PrivMsg(_srv);
	// _commands["NOTICE"] = new Notice(_srv);  
}

Parser::~Parser()
{
    for (comm_iter it = _commands.begin(); it != _commands.end(); ++it)
        delete it->second;
}



std::string     Parser::trim(const std::string& str)
{
    size_t start = str.find_first_not_of("\t\n\r\f\v");
    // If string only \t\n\r\f\v
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of("\t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

void Parser::invoke(Client* client, const std::string& message)
{

    std::stringstream ss(message);
    std::string syntax;

    while (std::getline(ss, syntax))
    {
        syntax = trim(syntax);
        std::string name = syntax.substr(0,syntax.find(' '));

        std::vector<std::string> args;
        std::stringstream line(syntax.substr(name.length(), syntax.length() - name.length()));
        std::string buf;

        try
        {
            Command *cmd = _commands.at(name);

            while (line >> buf)
                args.push_back(buf);

            // registartion check
            if (!client->is_registered() && cmd->get_auth())
            {
                client->reply(ERR_NOTREGISTERED(client->get_nickname()));
                return;
            }
            cmd->execute(client, args);
            
        }
        catch(const std::exception& e)
        {
           client->reply(ERR_UNKNOWNCOMMAND(client->get_nickname(), name));
        }
        
    }
    


}

