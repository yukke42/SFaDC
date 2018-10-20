/*
https://docs.opencv.org/3.4.3/db/df5/tutorial_linux_gcc_cmake.html
https://github.com/opencv/opencv/blob/master/samples/data/LinuxLogo.jpg
*/
#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat image = cv::imread("../LinuxLogo.jpg", 1);
    if (!image.data)
    {
        printf("No image data \n");
        return -1;
    }

    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Image", image);
    cv::waitKey(0);

    return 0;
}