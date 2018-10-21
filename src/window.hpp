/*
window.hpp

10 pixel (window) = 1 m (kitti datasets) 
*/

#ifndef WINDOW_CPP
#define WINDOW_CPP

#include <opencv2/opencv.hpp>

#define WINDOW_NAME "demo"
#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 600
#define WINDOW_X_AXIS WINDOW_WIDTH / 2
#define WINDOW_Z_AXIS WINDOW_HEIGHT - 100

#define BLACK cv::Scalar(0, 0, 0)
#define BLUE cv::Scalar(255, 0, 0)
#define GREEN cv::Scalar(0, 255, 0)
#define RED cv::Scalar(0, 0, 255)

class Window
{
  cv::Mat window;

public:
  Window()
  {
    window = cv::Mat::zeros(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3);
    Window::Refresh();
  };

  void Refresh();
  void Show();
  int WaitKey();

  void Rectangle(const float, const float, const float, const float);
  void PutFrameNoText(const int, const int);

private:
  int ToXWinCoord(const float);
  int ToZWinCoord(const float);
};

#endif