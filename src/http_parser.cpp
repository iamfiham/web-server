#include <string>
#include <vector>
#include <unordered_map>
#include <logger.hpp>

std::unordered_map<std::string, std::string> requestParser(const std::string &req)
{
    std::unordered_map<std::string, std::string> requestMap;
    std::string_view method(req.data(), req.find(" "));

    std::string_view endPoint(method.end() + 1, req.find(" ", method.size() + 1) - (method.size() + 1));

    requestMap["method"] = method;
    requestMap["end-point"] = endPoint;

    std::vector<size_t> positions;
    std::string pattern{"\r\n"};
    size_t pos = req.find(pattern);

    while (pos != std::string::npos)

    {
        positions.push_back(pos);
        pos = req.find(pattern, pos + pattern.size()); // search again starting after this match
    }

    // print results
    size_t prev = 0;
    for (size_t i = 0; i < positions.size() - 1; ++i)
    {

        if (i == 0)
        {
            prev = positions[i];
            continue;
        }

        std::string_view line(req.data() + prev + pattern.size(), positions[i] - prev - pattern.size());

        if (line.find(':') != std::string_view::npos)
        {
            int colon_index = line.find(':');

            std::string_view key(line.data(), colon_index);
            std::string_view value(line.data() + colon_index + 2, line.size() - colon_index - 2);

            requestMap[std::string(key)] = std::string(value);
        }
        prev = positions[i];
    }

    return requestMap;
}
