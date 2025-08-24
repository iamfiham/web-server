#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

class ManageSocket
{
private:
    int m_socket{};
    sockaddr_in m_server_address{};

public:
    ManageSocket(int port = 8080, in_addr_t addr = INADDR_ANY)
    {
        m_socket = socket(AF_INET, SOCK_STREAM, 0);

        m_server_address.sin_family = AF_INET;
        m_server_address.sin_port = htons(port);
        m_server_address.sin_addr.s_addr = addr;

#if 1
        // Allow quick rebinding after closing
        int opt = 1;
        if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            perror("setsockopt failed");
            exit(EXIT_FAILURE);
        }
#endif

        bind_socket();
        listen_socket();
    }

    void bind_socket()
    {
        if (bind(m_socket, reinterpret_cast<struct sockaddr *>(&m_server_address), sizeof(m_server_address)))
        {
            std::cerr << "[ERROR: binding error] " << strerror(errno) << '\n';
            close(m_socket);
            exit(EXIT_FAILURE);
        }
    }
    void listen_socket()
    {
        if (listen(m_socket, 5) == -1)
        {
            std::cerr << "[ERROR: listening error] " << strerror(errno) << '\n';
            close(m_socket);
            exit(EXIT_FAILURE);
        }
    }

    int getSocket()
    {
        return m_socket;
    }
};