#ifndef WINDOW_CPP
#define WINDOW_CPP

#define WINDOW_NAME "demo"
#define WINDOW_HEIGHT 500
#define WINDOW_WIDTH 500

#include <opencv2/opencv.hpp>

class Window
{
    cv::Mat window;

  public:
    Window()
    {
        window = cv::Mat::zeros(500, 500, CV_8UC3);
        Window::refresh();
    };
    void refresh();
    void show();
};

void Window::refresh()
{
    for (int j = 0; j < window.rows; j++)
    {
        for (int i = 0; i < window.cols; i++)
        {
            window.at<cv::Vec3b>(j, i)[0] = 255; // blue
            window.at<cv::Vec3b>(j, i)[1] = 255; // green
            window.at<cv::Vec3b>(j, i)[2] = 255; // red
        }
    }
}

void Window::show()
{
    cv::imshow(WINDOW_NAME, window);
    cv::waitKey(0);
}

#endif