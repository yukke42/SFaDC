/*
  window.hpp

  the sub window shows the pos of objects from bird's eye view
*/

#ifndef WINDOW_CPP
#define WINDOW_CPP

#include <opencv2/opencv.hpp>
#include <eigen3/Eigen/Dense>

#define WINDOW_NAME "demo"

// 10 pixel (a window) = 1 meter (kitti datasets)
#define METER_TO_PIXEL 10
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

  void Draw2DBoundingBox(const Eigen::MatrixXi, const cv::Scalar);

public:
  void ReadImage(const std::string);
  void InitSubWindow();
  void Concat();
  void Show();
  int WaitKey();

  void Draw2DBoundingBoxOnImage(const int, const int, const int, const int);
  void Draw3DBoundingBoxOnImage(const Eigen::MatrixXd);

  void Draw2DBoundingBoxBirdsView(Eigen::MatrixXd, const std::string);
  void PutImageIdText(const int, const int);
};

#endif