/*
    The coordinates in the camera coordinate system can be projected in the image
    by using the 3x4 projection matrix in the calib folder, where for the left
    color camera for which the images are provided, P2 must be used.
*/

#include "calib_parser.hpp"

#include <iostream>
#include <fstream>

#include "utils.hpp"

Eigen::MatrixXd ParseCalibFile(std::string filepath)
{
    std::ifstream stream(filepath);

    if (!stream.is_open())
    {
        std::cout << "File not Found: " << filepath << std::endl;
        std::exit(1);
    }

    std::string row;
    Eigen::MatrixXd ret(3, 4);
    for (int i = 0; i < 4 && getline(stream, row); i++)
    {
        if (i != 2)
            continue;

        std::vector<std::string> splited_row = split(row, ' ');

        ret << std::stod(splited_row[1]), std::stod(splited_row[2]), std::stod(splited_row[3]), std::stod(splited_row[4]),
            std::stod(splited_row[5]), std::stod(splited_row[6]), std::stod(splited_row[7]), std::stod(splited_row[8]),
            std::stod(splited_row[9]), std::stod(splited_row[10]), std::stod(splited_row[11]), std::stod(splited_row[12]);
    }

    return ret;
}