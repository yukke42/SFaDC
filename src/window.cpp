/*
    window.cpp
*/

#include "window.hpp"

#include "boost/format.hpp"

void Window::ReadImage(const std::string imageFilePath)
{
    window = cv::imread(imageFilePath);
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

    // 10 meter interval lines in depth, width
    for (int d = 10; d < SUB_WINDOW_HEIGHT / METER_TO_PIXEL; d += 10)
    {
        cv::line(sub_window,
                 cv::Point(0, SUB_WINDOW_Z_AXIS - d * METER_TO_PIXEL),
                 cv::Point(SUB_WINDOW_WIDTH, SUB_WINDOW_Z_AXIS - d * METER_TO_PIXEL),
                 BLACK,
                 1);
    }
    for (int w = 0; w < SUB_WINDOW_WIDTH / METER_TO_PIXEL; w += 10)
    {
        cv::line(sub_window,
                 cv::Point(w * METER_TO_PIXEL, 0),
                 cv::Point(w * METER_TO_PIXEL, SUB_WINDOW_HEIGHT),
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

void Window::Draw3DBoundingBoxOnImage(const Eigen::MatrixXd corners)
{
    const std::vector<std::vector<int>> connectedV{
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    int xPix1, yPix1, xPix2, yPix2;
    for (const auto &pVec : connectedV)
    {
        xPix1 = corners(0, pVec[0]) / corners(2, pVec[0]);
        yPix1 = corners(1, pVec[0]) / corners(2, pVec[0]);
        xPix2 = corners(0, pVec[1]) / corners(2, pVec[1]);
        yPix2 = corners(1, pVec[1]) / corners(2, pVec[1]);

        // std::cout << boost::format("x1: %f y1: %f x2: %f y2: %f\n") % xPix1 % yPix1 % xPix2 % yPix2;
        // TODO 物体の位置が画角にほとんど含まれていない場合にマイナスになることがある
        if (xPix1 < 0 || xPix2 < 0)
            break;

        cv::line(window,
                 cv::Point(xPix1, yPix1),
                 cv::Point(xPix2, yPix2),
                 GREEN);
    }
}

void Window::Draw2DBoundingBoxBirdsView(const Eigen::MatrixXd corners, const std::string color)
{
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

    Eigen::MatrixXd calibToBirdsViewMatrix(3, 4);
    calibToBirdsViewMatrix << METER_TO_PIXEL, 0, 0, SUB_WINDOW_X_AXIS,
        0, 0, 0, 0,
        0, 0, -METER_TO_PIXEL, SUB_WINDOW_Z_AXIS;

    Eigen::MatrixXd calibcorners = calibToBirdsViewMatrix * corners;

    int xPix1, zPix1, xPix2, zPix2;
    for (const auto &pVec : connectedV)
    {
        xPix1 = calibcorners(0, pVec[0]);
        zPix1 = calibcorners(2, pVec[0]);
        xPix2 = calibcorners(0, pVec[1]);
        zPix2 = calibcorners(2, pVec[1]);

        cv::line(sub_window,
                 cv::Point(xPix1, zPix1),
                 cv::Point(xPix2, zPix2),
                 COLOR,
                 2);
    }
}

void Window::PutImageIdText(const int frameNo, const int fraameLast)
{
    std::string text = std::to_string(frameNo) + " / " + std::to_string(fraameLast);
    cv::putText(sub_window, text, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 1, BLACK);
}
