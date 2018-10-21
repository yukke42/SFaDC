/*
window.cpp
*/

#include "window.hpp"

void Window::Refresh()
{
    // fill a window white
    for (int j = 0; j < window.rows; j++)
    {
        for (int i = 0; i < window.cols; i++)
        {
            window.at<cv::Vec3b>(j, i)[0] = 255;
            window.at<cv::Vec3b>(j, i)[1] = 255;
            window.at<cv::Vec3b>(j, i)[2] = 255;
        }
    }

    // draw xz-coordinate axis
    cv::line(window, cv::Point(0, WINDOW_Z_AXIS), cv::Point(WINDOW_WIDTH, WINDOW_Z_AXIS), BLACK);
    cv::line(window, cv::Point(WINDOW_X_AXIS, 0), cv::Point(WINDOW_X_AXIS, WINDOW_HEIGHT), BLACK);
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
    // transform camera coordinate to window coordinate
    const int xWinBottomLeft = ToXWinCoord(xCam - w / 2);
    const int zWinBottomLeft = ToZWinCoord(zCam - l / 2);
    const int xWinTopRight = ToXWinCoord(xCam + w / 2);
    const int zWinTopRight = ToZWinCoord(zCam + l / 2);

    cv::rectangle(window,
                  cv::Point(xWinBottomLeft, zWinBottomLeft),
                  cv::Point(xWinTopRight, zWinTopRight),
                  RED);
}

void Window::PutFrameNoText(const int frameNo, const int fraameLast)
{
    std::string text = std::to_string(frameNo) + " / " + std::to_string(fraameLast);
    cv::putText(window, text, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 1, BLACK);
}

int Window::ToXWinCoord(const float xCam)
{
    return (int)(xCam * 10) + WINDOW_X_AXIS;
}

int Window::ToZWinCoord(const float zCam)
{
    return -(int)(zCam * 10) + WINDOW_Z_AXIS;
}