#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

#include <manage_socket_class.hpp>
#include <request_handling.hpp>

int main()
{
	ManageSocket web_socket(8080, INADDR_ANY);

	while (true)
	{
		int client_socket = accept(web_socket.getSocket(), nullptr, nullptr);

		if (client_socket >= 0)
		{
			manage_request(client_socket);
		}
		else if (client_socket == -1)
		{
			std::cerr << "ERROR: client_socket error" << '\n';
		}
	}

	close(web_socket.getSocket());

	return 0;
}
