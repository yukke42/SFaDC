/*
  window.hpp

  10 pixel (a opencv window) = 1 meter (kitti datasets)

  the sub window shows the pos of objects from bird's eye view
*/

#ifndef WINDOW_CPP
#define WINDOW_CPP

#include <sstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#define WINDOW_NAME "demo"

#define SUB_WINDOW_HEIGHT 500
#define SUB_WINDOW_WIDTH 600
#define SUB_WINDOW_X_AXIS SUB_WINDOW_WIDTH / 2
#define SUB_WINDOW_Z_AXIS SUB_WINDOW_HEIGHT - 100

#define BLACK cv::Scalar(0, 0, 0)
#define BLUE cv::Scalar(255, 0, 0)
#define GREEN cv::Scalar(0, 255, 0)
#define RED cv::Scalar(0, 0, 255)

class Window
{
  cv::Mat window;
  cv::Mat sub_window;

public:
  void ReadImage(const unsigned int, const unsigned int);
  void InitSubWindow();
  void Concat();
  void Show();
  int WaitKey();

  void Rectangle(const float, const float, const float, const float);
  void PutImageIdText(const int, const int);

private:
  int ToXWinCoord(const float);
  int ToZWinCoord(const float);
};

#endif