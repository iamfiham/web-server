#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <memory>

#define PRINT(x) std::cout << x << '\n';

struct Response
{
private:
	std::vector<unsigned char> final_response_buffer;

public:
	bool success{false};

	Response(std::string &path, const std::string &content_type)
	{
		std::vector<std::string> header_array;
		std::string header_buffer;
		std::unique_ptr<std::vector<unsigned char>> body_buffer = std::make_unique<std::vector<unsigned char>>();

		if (path.empty())
		{
			path = "index.html";
		}

		std::ifstream input_file(path, std::ios::binary);
		if (!input_file.is_open())
		{
			std::cout << "failed to open " << path << '\n';
			return;
		}
		input_file.seekg(0, std::ios::end);
		int body_buffer_size = input_file.tellg();
		input_file.seekg(0, std::ios::beg);

		body_buffer->resize(body_buffer_size);
		input_file.read(reinterpret_cast<char *>(body_buffer->data()), body_buffer_size);

		header_array = {"HTTP/1.1 200 OK",
						"Content-Type: " + content_type,
						"Content-Length: " + std::to_string(body_buffer_size),
						"Connection : close"};

		auto construct_header_buffer = [&header_array, &header_buffer]()
		{
			for (auto &string : header_array)
				header_buffer += string + "\r\n";

			header_buffer += "\r\n";
		};

		construct_header_buffer();

		final_response_buffer.insert(final_response_buffer.end(), header_buffer.begin(), header_buffer.end());
		final_response_buffer.insert(final_response_buffer.end(), body_buffer->begin(), body_buffer->end());

		success = true;
	};

	const char *buffer_ptr()
	{
		return reinterpret_cast<char *>(final_response_buffer.data());
	};

	int buffer_size()
	{
		return final_response_buffer.size();
	};
};

int main()
{
	int webServerSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(webServerSocket, reinterpret_cast<struct sockaddr *>(&serverAddress), sizeof(serverAddress)))
	{
		std::cout << "ERROR: binding error" << '\n';
		return 1;
	}

	if (listen(webServerSocket, 5) == -1)
	{
		std::cout << "ERROR: listening error" << '\n';
		return 1;
	}

	while (true)
	{
		int clientSocket = accept(webServerSocket, nullptr, nullptr);

		if (clientSocket >= 0)
		{
			std::string request_buffer;
			request_buffer.resize(4000);
			int receivedBytes = recv(clientSocket, request_buffer.data(), request_buffer.capacity(), 0);
			request_buffer.resize(receivedBytes);

			auto find_the_requested_file = [&clientSocket, &request_buffer]()
			{
				int start_index = request_buffer.find('/');
				int end_index = request_buffer.find(' ', start_index);

				std::string text = request_buffer.substr(start_index + 1, end_index - start_index - 1);

				PRINT(text);

				Response response_obj(text, "text/html; charset=UTF-8");
				if (response_obj.success)
				{
					send(clientSocket, response_obj.buffer_ptr(), response_obj.buffer_size(), 0);
				}
			};

			if (receivedBytes >= 0)
			{
				request_buffer.resize(receivedBytes);
				PRINT(request_buffer);

				find_the_requested_file();
			}

			close(clientSocket);
		}
		else if (clientSocket == -1)
		{
			std::cout << "ERROR: clientSocket error" << '\n';
		}
	}

	close(webServerSocket);

	return 0;
}
