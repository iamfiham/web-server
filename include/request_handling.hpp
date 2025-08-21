#pragma once

#include <string>
#include <unordered_map>

#include <response_class.hpp>

std::string get_content_type(const std::string &path);
Response construct_response(const std::string &request_buffer);
void manage_request(int client_socket);