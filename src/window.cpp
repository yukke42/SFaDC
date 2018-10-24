/*
    window.cpp
*/

#include "window.hpp"

void Window::ReadImage(const std::string imageFilePath)
{
    window = cv::imread(imageFilePath);
    // std::cout << "size: " << window.size();
}

void Window::InitSubWindow()
{
    /*
        1. fill a sub window with white
        2. draw xz-coordinate axis
        3. put markers (TODO)
    */

    sub_window = cv::Mat::zeros(SUB_WINDOW_HEIGHT, window.cols, CV_8UC3);
    for (int j = 0; j < sub_window.rows; j++)
    {
        for (int i = 0; i < sub_window.cols; i++)
        {
            sub_window.at<cv::Vec3b>(j, i)[0] = 255;
            sub_window.at<cv::Vec3b>(j, i)[1] = 255;
            sub_window.at<cv::Vec3b>(j, i)[2] = 255;
        }
    }

    cv::line(sub_window,
             cv::Point(0, SUB_WINDOW_Z_AXIS),
             cv::Point(SUB_WINDOW_WIDTH, SUB_WINDOW_Z_AXIS),
             BLACK,
             2);
    cv::line(sub_window,
             cv::Point(SUB_WINDOW_X_AXIS, 0),
             cv::Point(SUB_WINDOW_X_AXIS, SUB_WINDOW_HEIGHT),
             BLACK,
             2);
    cv::line(sub_window,
             cv::Point(SUB_WINDOW_WIDTH, 0),
             cv::Point(SUB_WINDOW_WIDTH, SUB_WINDOW_HEIGHT),
             BLACK,
             2);

    // 10, 20, 30 meter lines in depth
    for (int i = 1; i < SUB_WINDOW_HEIGHT / (10 * PIXEL_M); i++)
    {
        cv::line(sub_window,
                 cv::Point(0, SUB_WINDOW_Z_AXIS - i * 10 * PIXEL_M),
                 cv::Point(SUB_WINDOW_WIDTH, SUB_WINDOW_Z_AXIS - i * 10 * PIXEL_M),
                 BLACK,
                 1);
    }
}

void Window::Concat()
{
    window.push_back(sub_window);
}

void Window::Show()
{
    cv::imshow(WINDOW_NAME, window);
}

int Window::WaitKey()
{
    return cv::waitKey();
}

void Window::DrawBoundingBoxImage(const int left, const int right, const int top, const int bottom)
{
    cv::line(window,
             cv::Point(right, top),
             cv::Point(left, top),
             RED,
             2);
    cv::line(window,
             cv::Point(left, top),
             cv::Point(left, bottom),
             RED,
             2);
    cv::line(window,
             cv::Point(left, bottom),
             cv::Point(right, bottom),
             RED,
             2);
    cv::line(window,
             cv::Point(right, bottom),
             cv::Point(right, top),
             RED,
             2);
}

void Window::DrawBoundingBox(const double xCam, const double zCam,
                             const double w, const double l,
                             const double yaw, const std::string color)
{
    /*
        1. calc the pos 
        2. transform camera coordinates to sub_window coordinates
        3. draw a rectangle
    */

    // === make codes simpler
    // Eigen::Matrix2d rotateMatrix;
    // const float angle_rad = -yaw + M_PI / 2;
    // rotateMatrix << std::cos(angle_rad), -std::sin(angle_rad),
    //     std::sin(angle_rad), std::cos(angle_rad);

    // Eigen::MatrixXd transformMatrix(2, 4);
    // transformMatrix << xCam, xCam, xCam, xCam,
    //     zCam, zCam, zCam, zCam;

    // // std::cout << transformMatrix << std::endl;

    // Eigen::MatrixXd cornersMatrixObjCoord(2, 4);
    // cornersMatrixObjCoord << w / 2, -w / 2, -w / 2, w / 2,
    //     l / 2, l / 2, -l / 2, -l / 2;

    // Eigen::MatrixXd cornersMatrixCamCoord(2, 4);
    // cornersMatrixCamCoord = rotateMatrix * cornersMatrixObjCoord + transformMatrix;

    // // std::cout << cornersMatrixCamCoord << std::endl;

    // rotateMatrix << 10, 0, 0, -10;
    // for (int j = 4; j < 4; j++)
    // {
    //     transformMatrix(0, j) = SUB_WINDOW_X_AXIS;
    // }

    // for (int j = 4; j < 4; j++)
    // {
    //     transformMatrix(0, j) = SUB_WINDOW_Z_AXIS;
    // }

    // Eigen::MatrixXi cornersMatrixWinCoord(2, 4);
    // cornersMatrixWinCoord = (rotateMatrix * cornersMatrixCamCoord + transformMatrix).cast<int>();

    // std::cout << cornersMatrixWinCoord << std::endl;
    // std::cout << cornersMatrixWinCoord(0) << std::endl;
    // std::cout << cornersMatrixWinCoord(7) << std::endl;

    // for (int i = 0; i < 4; i++)
    // {
    //     int next_i = (i + 1) % 4;
    //     cv::line(sub_window,
    //              cv::Point(cornersMatrixWinCoord(0, i), cornersMatrixWinCoord(1, i)),
    //              cv::Point(cornersMatrixWinCoord(1, next_i), cornersMatrixWinCoord(1, next_i)),
    //              RED,
    //              2);
    // }

    // =====================================

    Eigen::Vector2d objCenterCamCoord(xCam, zCam);
    Eigen::Matrix2d rotateMatrix;
    const float angle_rad = -yaw + M_PI / 2;
    rotateMatrix << std::cos(angle_rad), -std::sin(angle_rad),
        std::sin(angle_rad), std::cos(angle_rad);

    Eigen::Vector2d topRightObjCoord(w / 2, l / 2);
    Eigen::Vector2d topLeftObjCoord(-w / 2, l / 2);
    Eigen::Vector2d bottomLeftObjCoord(-w / 2, -l / 2);
    Eigen::Vector2d bottomRightObjCoord(w / 2, -l / 2);

    Eigen::Vector2d topRightCamCoord = rotateMatrix * topRightObjCoord + objCenterCamCoord;
    Eigen::Vector2d topLeftCamCoord = rotateMatrix * topLeftObjCoord + objCenterCamCoord;
    Eigen::Vector2d bottomLeftCamCoord = rotateMatrix * bottomLeftObjCoord + objCenterCamCoord;
    Eigen::Vector2d bottomRightCamCoord = rotateMatrix * bottomRightObjCoord + objCenterCamCoord;

    Eigen::Vector2d winCoordCenter(SUB_WINDOW_X_AXIS, SUB_WINDOW_Z_AXIS);
    rotateMatrix << PIXEL_M, 0, 0, -PIXEL_M;
    Eigen::Vector2i topRightWinCoord = (rotateMatrix * topRightCamCoord + winCoordCenter).cast<int>();
    Eigen::Vector2i topLeftWinCoord = (rotateMatrix * topLeftCamCoord + winCoordCenter).cast<int>();
    Eigen::Vector2i bottomLeftWinCoord = (rotateMatrix * bottomLeftCamCoord + winCoordCenter).cast<int>();
    Eigen::Vector2i bottomRightWinCoord = (rotateMatrix * bottomRightCamCoord + winCoordCenter).cast<int>();

    std::string red = "red";
    std::string blue = "blue";
    std::string green = "green";
    cv::Scalar COLOR;
    if (color == red)
        COLOR = RED;
    else if (color == blue)
        COLOR = BLUE;
    else if (color == green)
        COLOR = GREEN;

    cv::line(sub_window,
             cv::Point(topRightWinCoord[0], topRightWinCoord[1]),
             cv::Point(topLeftWinCoord[0], topLeftWinCoord[1]),
             COLOR,
             2);
    cv::line(sub_window,
             cv::Point(topLeftWinCoord[0], topLeftWinCoord[1]),
             cv::Point(bottomLeftWinCoord[0], bottomLeftWinCoord[1]),
             COLOR,
             2);
    cv::line(sub_window,
             cv::Point(bottomLeftWinCoord[0], bottomLeftWinCoord[1]),
             cv::Point(bottomRightWinCoord[0], bottomRightWinCoord[1]),
             COLOR,
             2);
    cv::line(sub_window,
             cv::Point(bottomRightWinCoord[0], bottomRightWinCoord[1]),
             cv::Point(topRightWinCoord[0], topRightWinCoord[1]),
             COLOR,
             2);
}

void Window::PutImageIdText(const int frameNo, const int fraameLast)
{
    std::string text = std::to_string(frameNo) + " / " + std::to_string(fraameLast);
    cv::putText(sub_window, text, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 1, BLACK);
}
