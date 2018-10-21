#include "utils.hpp"

// https://www.sejuku.net/blog/49378
std::vector<std::string> split(std::string str, char sep)
{
    long unsigned int s_start = 0;
    long unsigned int s_end = str.find_first_of(sep);

    std::vector<std::string> ret;

    while (s_start < str.size())
    {
        std::string sub_str(str, s_start, s_end - s_start);

        ret.push_back(sub_str);

        s_start = s_end + 1;
        s_end = str.find_first_of(sep, s_start);
        if (s_end == std::string::npos)
            s_end = str.size();
    }

    return ret;
}