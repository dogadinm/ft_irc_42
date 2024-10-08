#include "../../include/network/Server.hpp"

Server::Server(const std::string &port, const std::string &pass) : _host("127.0.0.1"), _port(port), _pass(pass)
{
    _working = 1;
    _socket = CreateSocket();
}

int Server::CreateSocket()
{
    int server_fd;
    struct sockaddr_in server_addr;  

    // Create the server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        throw std::runtime_error("Error, opening socket");


    // Set up the server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // Set the address family to IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to all available interfaces
    server_addr.sin_port = htons(atoi(_port.c_str())); // Convert the port to network byte order

    // Allow port reuse
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
        throw std::runtime_error("Error, socket options");

    // Make socket NON-BLOCKING
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK))
        throw std::runtime_error("Error, NON-BLOCKING doenst set");
    
    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        throw std::runtime_error("Error, binding socket");

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0)
        throw std::runtime_error("Error, listening socket");

    return server_fd; 
}

void            Server::start()
{
    pollfd srv = {_socket, POLLIN, 0};
    _plfds.push_back(srv);

    std::cout << "Server listening on port " << _port << std::endl;  
    while (_working)
    {
        if (poll(_plfds.begin().base(), _plfds.size(), -1) < 0)
            throw std::runtime_error("Error, polling!");
        
        // for (size_t i = 0; i < _plfds.size(); ++i) {
        // std::cout << "Index: " << i << ", fd: " << _plfds[i].fd
        //           << ", events: " << _plfds[i].events
        //           << ", revents: " << _plfds[i].revents << std::endl;
        // }

        for (plfds_iterator it = _plfds.begin(); it != _plfds.end(); it++)
        {
            if (it->revents == 0)
                 continue;
            if ((it->revents & POLLRDHUP ) == POLLRDHUP)
            {
                // std::cout << "POLLRDHUP" << std::endl;
                this->on_client_disconnect(it->fd);
                break;
            }

            if ((it->revents & POLLIN) == POLLIN)
            {
                std::cout << "POLLIN" << std::endl;
                if (it->fd == _socket)
                {
                    this->on_client_connect();
                    break;
                }

                this->on_client_message(it->fd);
            }
        }

    }
}

void Server::on_client_connect()
{
    int client_fd;
    struct sockaddr_in  client_addr;
    socklen_t client_addr_len = sizeof(client_addr);


    // Accept client fd
    client_fd = accept(_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0)
        throw std::runtime_error("Error, accepting a new client!");


    // Adding the client fd in the poll
    pollfd cln = {client_fd, POLLIN, 0};
    _plfds.push_back(cln);


    // Getting hostname from the client address
    char hostname[NI_MAXHOST];
    int res = getnameinfo((struct sockaddr *) &client_addr, sizeof(client_addr), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV);
    if (res != 0)
        throw std::runtime_error("Error, getting a hostname!");\

        
    std::string str(hostname);
    uint16_t t = ntohs(client_addr.sin_port); // port ?

    // doesnt make
    // Client* client = new Client(fd, ntohs(addr.sin_port), hostname);
    
    _clients.insert(std::make_pair(client_fd, t));
    std::cout << "Client connected" << std::endl;
}

void Server::on_client_disconnect(int fd)
{
    try
    {
        // need to make leave from the channel when user exit

        
        _clients.erase(fd);
        for(plfds_iterator it_b = _plfds.begin(); it_b != _plfds.end(); ++it_b)
        {
            if (it_b->fd == fd)
            {
                _plfds.erase(it_b);
                close(fd);
                break;
            }
        }
        std::cout << "Client disconnected" << std::endl;
    
    }
    catch(const std::exception& e)
    {
        std::cout << "Error while disconnecting! " << e.what() << std::endl;
    }
    

}

void Server::on_client_message(int fd)
{

    try
    {
        // Client*     client = _clients.at(fd);
       
        uint16_t clientData = _clients.at(fd);
        std::string message = this->read_message(fd);
        std::cout << clientData << ": " << message << std::endl;
        
        // _parser->invoke(client, message);
    }
    catch (const std::exception& e) 
    {
        std::cout << "Error while handling the client message! " << e.what() << std::endl;
    }
}

std::string Server::read_message(int fd)
{
    std::string message;
    
    char buffer[1024];
    bzero(buffer, 1024);

    while (!strstr(buffer, "\n"))
    {
        bzero(buffer, 100);
        ssize_t bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);

        if ((bytes_read < 0))
            throw std::runtime_error("Error while reading buffer from a client!");
        if (bytes_read == 0) {
            on_client_disconnect(fd);
            return "";
        }
        buffer[bytes_read] = '\0';
        message.append(buffer);
    }

    return message;
}





Server::~Server()
{
    if (_socket >= 0) {
        close(_socket);
    }
}


std::string Server::get_pass() const    { return _pass; }
