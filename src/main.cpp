#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <memory>

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

int main()
{
	int web_server_socket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8080);
	server_address.sin_addr.s_addr = INADDR_ANY;

	if (bind(web_server_socket, reinterpret_cast<struct sockaddr *>(&server_address), sizeof(server_address)))
	{
		std::cerr << "ERROR: binding error" << '\n';
		return 1;
	}

	if (listen(web_server_socket, 5) == -1)
	{
		std::cerr << "ERROR: listening error" << '\n';
		return 1;
	}

	while (true)
	{
		int client_socket = accept(web_server_socket, nullptr, nullptr);

		if (client_socket >= 0)
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
				PRINT_LINE(path);
				PRINT_LINE(content_type);
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
		else if (client_socket == -1)
		{
			std::cerr << "ERROR: client_socket error" << '\n';
		}
	}

	close(web_server_socket);

	return 0;
}
