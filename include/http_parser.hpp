#pragma once

#include <unordered_map>
#include <string>

std::unordered_map<std::string, std::string> requestParser(const std::string &req);
