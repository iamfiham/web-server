#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <memory>

#include <string.h>

#include <response.hpp>

#define PRINT_LINE(x) std::cout << x << '\n';

std::string get_content_type(const std::string &path)
{
	size_t dot_pos = path.find_last_of('.');
	if (dot_pos == std::string::npos)
		return "application/octet-stream";

	std::string ext = path.substr(dot_pos + 1);
	if (ext == "html")
		return "text/html; charset=UTF-8";
	if (ext == "css")
		return "text/css; charset=UTF-8";
	if (ext == "js")
		return "application/javascript; charset=UTF-8";
	if (ext == "png")
		return "image/png";
	if (ext == "jpg" || ext == "jpeg")
		return "image/jpeg";
	if (ext == "gif")
		return "image/gif";
	if (ext == "bmp")
		return "image/bmp";
	if (ext == "ico")
		return "image/x-icon";

	return "application/octet-stream";
};

void manageRequest(int client_socket)
{
	std::string request_buffer;
	request_buffer.resize(4000);

	int received_bytes = recv(client_socket, request_buffer.data(), request_buffer.capacity(), 0);
	request_buffer.resize(received_bytes);

	auto find_the_requested_file = [&client_socket, &request_buffer]()
	{
		int start_index = request_buffer.find('/');
		int end_index = request_buffer.find(' ', start_index);

		std::string path = request_buffer.substr(start_index + 1, end_index - start_index - 1);

		if (path.empty())
		{
			path = "index.html";
		}

		std::string content_type = get_content_type(path);

		PRINT_LINE("--------------------------");

		Response response_obj(path, content_type);

		if (response_obj.success)
		{
			send(client_socket, response_obj.buffer_ptr(), response_obj.buffer_size(), 0);
		}
	};

	if (received_bytes >= 0)
	{
		request_buffer.resize(received_bytes);
		PRINT_LINE(request_buffer);

		find_the_requested_file();
	}

	close(client_socket);
}

struct ManageSocket
{
	int _socket;
	sockaddr_in server_address;

	ManageSocket(int port = 8080, in_addr_t addr = INADDR_ANY)
	{
		_socket = socket(AF_INET, SOCK_STREAM, 0);

		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(port);
		server_address.sin_addr.s_addr = addr;

		bind_socket();
		listen_socket();
	}

	void bind_socket()
	{
		if (bind(_socket, reinterpret_cast<struct sockaddr *>(&server_address), sizeof(server_address)))
		{
			std::cerr << "[ERROR: binding error] " << strerror(errno) << '\n';
			close(_socket);
			exit(EXIT_FAILURE);
		}
	}
	void listen_socket()
	{
		if (listen(_socket, 5) == -1)
		{
			std::cerr << "[ERROR: listening error] " << strerror(errno) << '\n';
			close(_socket);
			exit(EXIT_FAILURE);
		}
	}
};

int main()
{
	ManageSocket web_socket(INADDR_ANY, 8080);

	while (true)
	{
		int client_socket = accept(web_socket._socket, nullptr, nullptr);

		if (client_socket >= 0)
		{
			manageRequest(client_socket);
		}
		else if (client_socket == -1)
		{
			std::cerr << "ERROR: client_socket error" << '\n';
		}
	}

	close(web_socket._socket);

	return 0;
}
