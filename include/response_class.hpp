#pragma once

#include <iostream>
#include <netinet/in.h>
#include <fstream>
#include <vector>
#include <memory>

class Response
{
private:
    using uCharVect = std::vector<unsigned char>;

private:
    bool m_success{false};
    uCharVect response_buffer;

public:
    Response(const std::string &path, const std::string &content_type)
    {
        std::vector<std::string> header_string_array;
        std::string header_buffer;
        std::unique_ptr<uCharVect> body_buffer{std::make_unique<uCharVect>()};

        std::string base_location = "static";

        std::ifstream input_file(base_location + '/' + path, std::ios::binary);
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

        header_string_array = {"HTTP/1.1 200 OK",
                               "Content-Type: " + content_type,
                               "Content-Length: " + std::to_string(body_buffer_size),
                               "Connection : close"};

        auto construct_header_buffer{
            [&header_string_array, &header_buffer]()
            {
                for (auto &string : header_string_array)
                    header_buffer += string + "\r\n";

                header_buffer += "\r\n";
            }};

        construct_header_buffer();

        response_buffer.insert(response_buffer.end(), header_buffer.begin(), header_buffer.end());
        response_buffer.insert(response_buffer.end(), body_buffer->begin(), body_buffer->end());

        m_success = true;
    };

    const char *begin()
    {
        return reinterpret_cast<char *>(response_buffer.data());
    };

    int size()
    {
        return response_buffer.size();
    };

    bool success()
    {
        return m_success;
    }
};