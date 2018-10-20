/*
10 pixel (window) = 1 m (kitti datasets) 
*/

#ifndef WINDOW_CPP
#define WINDOW_CPP

#define WINDOW_NAME "demo"
#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 600
#define WINDOW_X_AXIS WINDOW_WIDTH / 2
#define WINDOW_Z_AXIS WINDOW_HEIGHT - 100

#define BLACK cv::Scalar(0, 0, 0)
#define BLUE cv::Scalar(255, 0, 0)
#define GREEN cv::Scalar(0, 255, 0)
#define RED cv::Scalar(0, 0, 255)

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

    void rectangle(float, float, float, float);

  private:
    int to_x_win_coord(float);
    int to_z_win_coord(float);
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

    // draw xz-coordinate axis
    cv::line(window, cv::Point(0, WINDOW_Z_AXIS), cv::Point(WINDOW_WIDTH, WINDOW_Z_AXIS), BLACK);
    cv::line(window, cv::Point(WINDOW_X_AXIS, 0), cv::Point(WINDOW_X_AXIS, WINDOW_HEIGHT), BLACK);
}

void Window::show()
{
    cv::imshow(WINDOW_NAME, window);
    cv::waitKey(0);
}

void Window::rectangle(float x_c, float z_c, float w, float l)
{
    // point x_c, z_c is the center of the object from bird's eye view
    int x_win_bottom_left = to_x_win_coord(x_c - w / 2);
    int z_win_bottom_left = to_z_win_coord(z_c - l / 2);
    int x_win_top_right = to_x_win_coord(x_c + w / 2);
    int z_win_top_right = to_x_win_coord(z_c + l / 2);

    cv::rectangle(window,
                  cv::Point(x_win_bottom_left, z_win_bottom_left),
                  cv::Point(x_win_top_right, z_win_top_right),
                  RED);
}

int Window::to_x_win_coord(float x_c)
{
    return (int)(x_c * 10) + WINDOW_X_AXIS;
}

int Window::to_z_win_coord(float z_c)
{
    return -(int)(z_c * 10) + WINDOW_Z_AXIS;
}

#endif