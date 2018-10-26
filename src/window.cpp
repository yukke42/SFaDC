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

void Window::Draw2DBoundingBoxOnImage(const int left, const int right, const int top, const int bottom)
{
    cv::rectangle(window,
                  cv::Point(right, top),
                  cv::Point(left, bottom),
                  RED);
}

void Window::Draw3DBoundingBoxOnImage(const Eigen::MatrixXi corners)
{
    const std::vector<std::vector<int>> connectedV{
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 6}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    int xPix1, yPix1, xPix2, yPix2;
    for (const auto &pVec : connectedV)
    {
        if (corners(2, pVec[0]) == 0 || corners(2, pVec[1]) == 0)
        {
            std::cout << "zero" << std::endl;
            // std::cout << corners << std::endl;
            break;
        }
        xPix1 = corners(0, pVec[0]) / corners(2, pVec[0]);
        yPix1 = corners(1, pVec[0]) / corners(2, pVec[0]);
        xPix2 = corners(0, pVec[1]) / corners(2, pVec[1]);
        yPix2 = corners(1, pVec[1]) / corners(2, pVec[1]);
        // std::cout << xPix1 << std::endl;
        cv::line(window,
                 cv::Point(xPix1, yPix1),
                 cv::Point(xPix2, yPix2),
                 GREEN,
                 1);
    }
}

void Window::Draw2DBoundingBoxBirdsView(const double xCam, const double zCam,
                                        const double w, const double l,
                                        const double yaw, const std::string color)
{
    /*
        1. calc the pos 
        2. transform camera coordinates to sub_window coordinates
        3. draw a rectangle
    */

    Eigen::Matrix2d rotateMatrix;
    Eigen::MatrixXd transformMatrix(2, 4);
    const float angle_rad = -yaw + M_PI / 2;
    rotateMatrix << std::cos(angle_rad), -std::sin(angle_rad),
        std::sin(angle_rad), std::cos(angle_rad);
    for (int i = 0; i < 4; i++)
    {
        transformMatrix(0, i) = xCam;
        transformMatrix(1, i) = zCam;
    }

    const std::vector<double> x_corners{w / 2, -w / 2, -w / 2, w / 2};
    const std::vector<double> z_corners{l / 2, l / 2, -l / 2, -l / 2};
    Eigen::MatrixXd cornersMatrixObjCoord(2, 4);
    for (int i = 0; i < 4; i++)
    {
        cornersMatrixObjCoord(0, i) = x_corners[i];
        cornersMatrixObjCoord(1, i) = z_corners[i];
    }
    // std::cout << cornersMatrixObjCoord << std::endl;

    Eigen::MatrixXd cornersMatrixCamCoord(2, 4);
    cornersMatrixCamCoord = rotateMatrix * cornersMatrixObjCoord + transformMatrix;

    rotateMatrix << PIXEL_M, 0, 0, -PIXEL_M;
    for (int j = 0; j < 4; j++)
    {
        transformMatrix(0, j) = SUB_WINDOW_X_AXIS;
        transformMatrix(1, j) = SUB_WINDOW_Z_AXIS;
    }

    Eigen::MatrixXi cornersMatrixPixCoord(2, 4);
    cornersMatrixPixCoord = (rotateMatrix * cornersMatrixCamCoord + transformMatrix).cast<int>();

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

    const std::vector<std::vector<int>> connectedV{
        {0, 1}, {1, 2}, {2, 3}, {3, 0}};
    int xPix1, yPix1, xPix2, yPix2;
    for (const auto &pVec : connectedV)
    {
        xPix1 = cornersMatrixPixCoord(0, pVec[0]);
        yPix1 = cornersMatrixPixCoord(1, pVec[0]);
        xPix2 = cornersMatrixPixCoord(0, pVec[1]);
        yPix2 = cornersMatrixPixCoord(1, pVec[1]);
        cv::line(sub_window,
                 cv::Point(xPix1, yPix1),
                 cv::Point(xPix2, yPix2),
                 COLOR,
                 2);
    }
}

void Window::PutImageIdText(const int frameNo, const int fraameLast)
{
    std::string text = std::to_string(frameNo) + " / " + std::to_string(fraameLast);
    cv::putText(sub_window, text, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 1, BLACK);
}
