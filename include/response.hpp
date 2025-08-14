#pragma once

#include <iostream>
#include <netinet/in.h>
#include <fstream>
#include <vector>
#include <memory>

struct Response
{
private:
    std::vector<unsigned char> final_response_buffer;

public:
    bool success{false};

    Response(const std::string &path, const std::string &content_type)
    {
        std::vector<std::string> header_array;
        std::string header_buffer;
        std::unique_ptr<std::vector<unsigned char>> body_buffer = std::make_unique<std::vector<unsigned char>>();

        std::string base_location = "static/";

        std::ifstream input_file(base_location + path, std::ios::binary);
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