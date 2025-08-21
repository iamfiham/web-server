#include <iostream>
#include <string>

#include <netinet/in.h>
#include <response_class.hpp>
#include <unistd.h>

#include <request_handling.hpp>
#include <unordered_map>

#define PRINT_LINE(x) std::cout << x << '\n';

std::unordered_map<std::string, std::string> content_type_map{
    {"html", "text/html; charset=UTF-8"},
    {"css", "text/css; charset=UTF-8"},
    {"js", "application/javascript; charset=UTF-8"},
    {"png", "image/png"},
    {"jpg", "image/jpg"},
    {"jpeg", "image/jpeg"},
    {"gif", "image/gif"},
    {"bmp", "image/bmp"},
    {"ico", "image/x-icon"}};

std::string get_content_type(const std::string &path)
{
    size_t dot_index = path.find_last_of('.');
    if (dot_index == std::string::npos)
        return "application/octet-stream";

    std::string ext = path.substr(dot_index + 1);
    auto element_pos = content_type_map.find(ext);

    if (element_pos != content_type_map.end())
        return element_pos->second;
    else
        return "application/octet-stream";
};

Response construct_response(const std::string &request_buffer)
{
    int start_index = request_buffer.find('/');
    int end_index = request_buffer.find(' ', start_index);

    std::string path = request_buffer.substr(start_index + 1, end_index - start_index - 1);

    if (path.empty())
        path = "index.html";

    std::string content_type = get_content_type(path);

    return Response(path, content_type);
};

void manage_request(int client_socket)
{
    std::string request_buffer{};
    request_buffer.resize(4000);

    int received_bytes = recv(client_socket, request_buffer.data(), request_buffer.capacity(), 0);

    if (received_bytes >= 0)
    {
        request_buffer.resize(received_bytes);

        PRINT_LINE(request_buffer);
        PRINT_LINE("---------------------------");

        Response response_obj{construct_response(request_buffer)};

        if (response_obj.success())
        {
            send(client_socket, response_obj.begin(), response_obj.size(), 0);
        }
    }

    close(client_socket);
}