/*
demo.cpp

reference
- http://beetreehitsuji.hatenablog.com/entry/2015/01/12/223230
*/

#include "demo.hpp"

#include <string>
#include <opencv2/opencv.hpp>

int main()
{

    cv::Mat image = cv::Mat::zeros(500, 500, CV_8UC3);
    for (int j = 0; j < image.rows; j++)
    {
        for (int i = 0; i < image.cols; i++)
        {
            image.at<cv::Vec3b>(j, i)[0] = 255; // blue
            image.at<cv::Vec3b>(j, i)[1] = 255; // green
            image.at<cv::Vec3b>(j, i)[2] = 255; // red
        }
    }

    cv::namedWindow(WINDOW_NAME);
    cv::imshow(WINDOW_NAME, image);
    cv::waitKey(0);

    return 0;
}
