#include "label_parser.hpp"

#include <iostream>
#include <fstream>

#include "utils.hpp"

std::vector<Tracklet> ParseLabelFile(std::string filepath)
{
    std::ifstream stream(filepath);

    if (!stream.is_open())
    {
        std::cout << "File not Found: " << filepath << std::endl;
        std::exit(1);
    }

    std::string row;
    std::vector<Tracklet> tracklet_vec;
    while (getline(stream, row))
    {
        std::vector<std::string> splited_row = split(row, ' ');

        float score = -1;
        if (splited_row.size() == 18)
            score = std::stof(splited_row[17]);

        Tracklet tracklet_tmp = {
            (unsigned int)std::stoi(splited_row[0]),
            std::stoi(splited_row[1]),
            splited_row[2],
            std::stoi(splited_row[3]),
            std::stoi(splited_row[4]),
            std::stof(splited_row[5]),
            std::stof(splited_row[6]),
            std::stof(splited_row[7]),
            std::stof(splited_row[8]),
            std::stof(splited_row[9]),
            std::stof(splited_row[10]),
            std::stof(splited_row[11]),
            std::stof(splited_row[12]),
            std::stof(splited_row[13]),
            std::stof(splited_row[14]),
            std::stof(splited_row[15]),
            std::stof(splited_row[16]),
            score,
        };

        tracklet_vec.push_back(tracklet_tmp);
    }

    return tracklet_vec;
}