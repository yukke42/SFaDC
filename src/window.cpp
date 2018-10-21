/*
    window.cpp
*/

#include "window.hpp"

void Window::ReadImage(const unsigned int imageSetsId, const unsigned int imageId)
{
    const std::string imagesDirRoot = "../../mini_datasets/training/image_02/";
    std::ostringstream image_sets_id_str, image_id_str;
    image_sets_id_str << std::setw(4) << std::setfill('0') << imageSetsId;
    image_id_str << std::setw(6) << std::setfill('0') << imageId;
    const std::string imageFilePath = imagesDirRoot + image_sets_id_str.str() + "/" + image_id_str.str() + ".png";
    std::cout << "Read image: " << imageFilePath << std::endl;
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
             BLACK);
    cv::line(sub_window,
             cv::Point(SUB_WINDOW_X_AXIS, 0),
             cv::Point(SUB_WINDOW_X_AXIS, SUB_WINDOW_HEIGHT),
             BLACK);
    cv::line(sub_window,
             cv::Point(SUB_WINDOW_WIDTH, 0),
             cv::Point(SUB_WINDOW_WIDTH, SUB_WINDOW_HEIGHT),
             BLACK,
             2);
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

void Window::Rectangle(const float xCam, const float zCam, const float w, const float l)
{
    /*
        1. calc the pos and angle of a object (TODO)
        2. transform camera coordinates to sub_window coordinates
        3. draw a rectangle
    */

    const int xWinBottomLeft = ToXWinCoord(xCam - w / 2);
    const int zWinBottomLeft = ToZWinCoord(zCam - l / 2);
    const int xWinTopRight = ToXWinCoord(xCam + w / 2);
    const int zWinTopRight = ToZWinCoord(zCam + l / 2);

    cv::rectangle(sub_window,
                  cv::Point(xWinBottomLeft, zWinBottomLeft),
                  cv::Point(xWinTopRight, zWinTopRight),
                  RED);
}

void Window::PutImageIdText(const int frameNo, const int fraameLast)
{
    std::string text = std::to_string(frameNo) + " / " + std::to_string(fraameLast);
    cv::putText(sub_window, text, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 1, BLACK);
}

int Window::ToXWinCoord(const float xCam)
{
    return (int)(xCam * 10) + SUB_WINDOW_X_AXIS;
}

int Window::ToZWinCoord(const float zCam)
{
    return -(int)(zCam * 10) + SUB_WINDOW_Z_AXIS;
}