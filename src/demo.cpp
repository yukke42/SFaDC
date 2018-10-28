/*
    demo.cpp
*/

#include "boost/format.hpp"

#include <iostream>
#include <vector>

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/LU>

#include "calib_parser.hpp"
#include "label_parser.hpp"
#include "window.hpp"

#define HEIGHT 1.73 // the height of camera
#define THETA 0     // the ground plane pitch angle

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: ./demo IMAGE_SET_ID(0-20)" << std::endl;
        return 1;
    }
    const unsigned int imageSetsId = atoi(argv[1]);

    const std::string dataSetsRootDir = "../../datasets/training";
    const std::string calibFilePath = str(boost::format("%s/calib/%04d.txt") % dataSetsRootDir % imageSetsId);
    const std::string labelFilePath = str(boost::format("%s/label_02/%04d.txt") % dataSetsRootDir % imageSetsId);
    const Eigen::MatrixXd calibToImageAffineMatrix = ParseCalibFile(calibFilePath);
    const std::vector<Tracklet> tracklets = ParseLabelFile(labelFilePath);

    Eigen::MatrixXd calibToImageRotateMatrix(3, 3);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            calibToImageRotateMatrix(i, j) = calibToImageAffineMatrix(i, j);
        }
    }

    Window win;

    unsigned int image_id = 0;
    const unsigned int imageLast = tracklets[tracklets.size() - 1].image_id;
    std::string imageFilePath;
    double x, y, z, l, h, w, yaw_rad;
    Eigen::MatrixXd corners3DBBObjHomoCoordMatrix(4, 8);
    Eigen::MatrixXd corners3DBBCamHomoCoordMatrix(4, 8);
    Eigen::MatrixXd corners3DBBCamCoordMatrix(3, 8);
    Eigen::MatrixXd affineYAxisMatrix(4, 4);
    while (1)
    {
        /*
            1. read an image and crate a main window
            2. create a sub window (show Bounding Boxes of objects from the bird's eye view)
            3. draw obj pos to the sub window
            4. concat them
            5. show the window
            6. wait a keybord input
        */

        std::cout << "Image: " << image_id << std::endl;
        imageFilePath = str(boost::format("%s/image_02/%04d/%06d.png") %
                            dataSetsRootDir %
                            imageSetsId %
                            image_id);

        win.ReadImage(imageFilePath);
        win.InitSubWindow();
        win.PutImageIdText(image_id, imageLast);

        for (unsigned int obj_i = 1; obj_i < tracklets.size(); obj_i++)
        {

            if (tracklets[obj_i].image_id != image_id)
                continue;
            if (tracklets[obj_i].obj_type == "DontCare")
                continue;
            if (tracklets[obj_i].occluded == 0)
                continue;
            if (tracklets[obj_i].truncated == 1)
                continue;

            std::cout << tracklets[obj_i].obj_type
                      << " "
                      << tracklets[obj_i].track_id
                      << std::endl;

            // === calc the 3D BB on the camera coordinates ===
            // 1. init homogeneous coordinates of a object
            // 2. affine transformation around y axis
            //    translation is from object coordinates to camera coordinates

            x = tracklets[obj_i].x_3d, y = tracklets[obj_i].y_3d, z = tracklets[obj_i].z_3d;
            l = tracklets[obj_i].l_3d, h = tracklets[obj_i].h_3d, w = tracklets[obj_i].w_3d;

            // std::cout << boost::format("x: %f y: %f z: %f\n") % x % y % z;
            // std::cout << boost::format("l: %f h: %f w: %f\n") % l % h % w;
            // std::cout << boost::format("yaw: %f\n") % tracklets[obj_i].yaw_3d;
            const std::vector<double> x_corners{l / 2, l / 2, -l / 2, -l / 2,
                                                l / 2, l / 2, -l / 2, -l / 2};
            const std::vector<double> y_corners{0, 0, 0, 0, -h, -h, -h, -h};
            const std::vector<double> z_corners{w / 2, -w / 2, -w / 2, w / 2,
                                                w / 2, -w / 2, -w / 2, w / 2};
            for (int i = 0; i < 8; i++)
            {
                corners3DBBObjHomoCoordMatrix(0, i) = x_corners[i];
                corners3DBBObjHomoCoordMatrix(1, i) = y_corners[i];
                corners3DBBObjHomoCoordMatrix(2, i) = z_corners[i];
                corners3DBBObjHomoCoordMatrix(3, i) = 1;
            }

            yaw_rad = tracklets[obj_i].yaw_3d;
            affineYAxisMatrix << std::cos(yaw_rad), 0, std::sin(yaw_rad), x,
                0, 1, 0, y,
                -std::sin(yaw_rad), 0, std::cos(yaw_rad), z,
                0, 0, 0, 1;
            corners3DBBCamHomoCoordMatrix = affineYAxisMatrix * corners3DBBObjHomoCoordMatrix;
            // ================================================

            // === draw the BB on the image ===
            win.Draw3DBoundingBoxOnImage(calibToImageAffineMatrix * corners3DBBCamHomoCoordMatrix);

            win.Draw2DBoundingBoxOnImage((int)tracklets[obj_i].x_2d_left,
                                         (int)tracklets[obj_i].x_2d_right,
                                         (int)tracklets[obj_i].y_2d_top,
                                         (int)tracklets[obj_i].y_2d_bottom);
            // =================================

            // === draw the 2D BB on the Bird's view window ===
            win.Draw2DBoundingBoxBirdsView(corners3DBBCamHomoCoordMatrix,
                                           "red");
            // ================================================

            // === calc the obj pos on the ground plane ===
            // with a fixed ground plane
            Eigen::Vector3d bottomCenter2DBBPixHomoCoord(
                (tracklets[obj_i].x_2d_left + tracklets[obj_i].x_2d_right) / 2,
                tracklets[obj_i].y_2d_bottom,
                1);

            // the normal of the ground plane
            Eigen::Vector3d N(0, -std::cos(THETA), std::sin(THETA));

            // back-projection from 2D to 3D
            Eigen::Vector3d calibrated =
                calibToImageRotateMatrix.inverse() * bottomCenter2DBBPixHomoCoord;
            Eigen::Vector3d bottomCenter3DBBCamCoord =
                -HEIGHT * calibrated / N.dot(calibrated);

            // calc the center of the object
            Eigen::Vector3d bottomToCenter(0, 0, w / 2);
            Eigen::Matrix3d rotateYAxis;
            rotateYAxis << std::cos(yaw_rad), 0, std::sin(yaw_rad),
                0, 1, 0,
                -std::sin(yaw_rad), 0, std::cos(yaw_rad);

            Eigen::Vector3d center3DBBCamCoord =
                rotateYAxis * bottomToCenter + bottomCenter3DBBCamCoord;

            // std::cout << boost::format("true x: %f y: %f z: %f\n") % x % y % z;
            // std::cout << boost::format("calc x: %f y: %f z: %f\n") %
            //                  center3DBBCamCoord[0] %
            //                  center3DBBCamCoord[1] %
            //                  center3DBBCamCoord[2];

            // calc 4 corners coordinate from bird's view
            affineYAxisMatrix << std::cos(yaw_rad), 0, std::sin(yaw_rad), center3DBBCamCoord[0],
                0, 1, 0, center3DBBCamCoord[1],
                -std::sin(yaw_rad), 0, std::cos(yaw_rad), center3DBBCamCoord[2],
                0, 0, 0, 1;

            win.Draw2DBoundingBoxBirdsView(
                affineYAxisMatrix * corners3DBBObjHomoCoordMatrix,
                "green");
        }

        win.Concat();
        win.Show();

        while (int pressed_key = (win.WaitKey() & 0xff))
        {
            // std::cout << "pressed key num: " << pressed_key << std::endl;
            if (pressed_key == 113) // q
            {
                return 0;
            }
            else if (pressed_key == 83) // →
            {
                if (image_id < imageLast)
                    image_id++;
                break;
            }
            else if (pressed_key == 81) // ←
            {
                if (image_id > 1)
                    image_id--;
                break;
            }
        }
    }

    return 0;
}
