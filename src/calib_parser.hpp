#ifndef CALIB_PARSER_HPP
#define CALIB_PARSER_HPP

#include <string>
#include <vector>

#include <eigen3/Eigen/Dense>

Eigen::MatrixXd ParseCalibFile(std::string);

#endif