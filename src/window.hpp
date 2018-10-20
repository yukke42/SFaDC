#ifndef WINDOW_CPP
#define WINDOW_CPP

#define WINDOW_NAME "demo"
#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 600
#define WINDOW_X_CENTER WINDOW_WIDTH / 2
#define WINDOW_Y_CENTER WINDOW_HEIGHT - 100

#define BLACK cv::Scalar(0, 0, 0)

#include <opencv2/opencv.hpp>

class Window
{
    cv::Mat window;

  public:
    Window()
    {
        window = cv::Mat::zeros(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3);
        Window::refresh();
    };
    void refresh();
    void show();
};

void Window::refresh()
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

    // draw yz-coordinate axis
    cv::line(window, cv::Point(0, WINDOW_Y_CENTER), cv::Point(WINDOW_WIDTH, WINDOW_Y_CENTER), BLACK);
    cv::line(window, cv::Point(WINDOW_X_CENTER, 0), cv::Point(WINDOW_X_CENTER, WINDOW_HEIGHT), BLACK);
}

void Window::show()
{
    cv::imshow(WINDOW_NAME, window);
    cv::waitKey(0);
}

#endif