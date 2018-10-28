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

#define HEIGHT 1.73

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: ./demo IMAGE_SET_ID" << std::endl;
        return 1;
    }
    const unsigned int imageSetsId = atoi(argv[1]);

    const std::string dataSetsRootDir = "../../datasets/training";
    std::string labelFilePath = str(boost::format("%s/label_02/%04d.txt") % dataSetsRootDir % imageSetsId);
    std::vector<Tracklet> tracklets = ParseLabelFile(labelFilePath);
    std::string calibFilePath = str(boost::format("%s/calib/%04d.txt") % dataSetsRootDir % imageSetsId);
    const Eigen::MatrixXd calibToImageMatrix3D = ParseCalibFile(calibFilePath);

    // std::cout << calibrationMatrix3D << std::endl;
    // std::cout << CalibrationMatrix.rows() << std::endl;
    // std::cout << CalibrationMatrix.cols() << std::endl;
    // std::cout << CalibrationMatrix(0, 0) << std::endl;
    // Eigen::MatrixXd intrinsicCalibrationMatrix2D(3, 3);
    // intrinsicCalibrationMatrix2D << calibrationMatrix3D(0, 0), calibrationMatrix3D(0, 1), calibrationMatrix3D(0, 2),
    //     calibrationMatrix3D(1, 0), calibrationMatrix3D(1, 1), calibrationMatrix3D(1, 2),
    //     calibrationMatrix3D(2, 0), calibrationMatrix3D(2, 1), calibrationMatrix3D(2, 2);

    // Eigen::Vector3d intrinsicT(calibrationMatrix3D(0, 3), calibrationMatrix3D(1, 3), calibrationMatrix3D(2, 3));

    // std::cout << CalibrationMatrix2D << std::endl;
    // std::cout << CalibrationMatrix2D.inverse() << std::endl;

    Window win;

    unsigned int image_id = 0;
    const unsigned int imageLast = tracklets[tracklets.size() - 1].image_id;
    std::string imageFilePath;
    double x, y, z, l, h, w, yaw;
    Eigen::MatrixXd corners3DBBObjHomoCoordMatrix(4, 8);
    Eigen::MatrixXd corners3DBBCamHomoCoordMatrix(4, 8);
    Eigen::Matrix4d affineYAxisMatrix(4, 4);
    Eigen::MatrixXd calibToBirdsViewMatrix(3, 4);
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

        // std::cout << "Image: " << image_id << std::endl;
        imageFilePath = str(boost::format("%s/image_02/%04d/%06d.png") % dataSetsRootDir % imageSetsId % image_id);

        win.ReadImage(imageFilePath);
        win.InitSubWindow();
        win.PutImageIdText(image_id, imageLast);

        for (unsigned int obj_i = 1; obj_i < tracklets.size(); obj_i++)
        {

            if (tracklets[obj_i].image_id != image_id)
                continue;
            if (tracklets[obj_i].obj_type == "DontCare")
                continue;
            if (tracklets[obj_i].occluded > 2)
                continue;
            if (tracklets[obj_i].truncated == 1)
                continue;

            // === calc the 3D BB on the camera coordinates ===
            // 1. init homogeneous coordinates of a object
            // 2. rotate it around y axis
            // 3. transform it from object coordinates to camera coordinates

            x = tracklets[obj_i].x_3d, y = tracklets[obj_i].y_3d, z = tracklets[obj_i].z_3d;
            l = tracklets[obj_i].l_3d, h = tracklets[obj_i].h_3d, w = tracklets[obj_i].w_3d;

            // std::cout << boost::format("x: %f y: %f z: %f\n") % z % y % z;
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

            yaw = tracklets[obj_i].yaw_3d;
            affineYAxisMatrix << std::cos(yaw), 0, std::sin(yaw), x,
                0, 1, 0, y,
                -std::sin(yaw), 0, std::cos(yaw), z,
                0, 0, 0, 1;
            corners3DBBCamHomoCoordMatrix = affineYAxisMatrix * corners3DBBObjHomoCoordMatrix;
            // ================================================

            // === draw the BB on the image ===
            win.Draw3DBoundingBoxOnImage(calibToImageMatrix3D * corners3DBBCamHomoCoordMatrix);

            win.Draw2DBoundingBoxOnImage(
                (int)tracklets[obj_i].x_2d_left,
                (int)tracklets[obj_i].x_2d_right,
                (int)tracklets[obj_i].y_2d_top,
                (int)tracklets[obj_i].y_2d_bottom);
            // =================================

            // === draw the 2D BB on the Bird's view window ===
            calibToBirdsViewMatrix << METER_TO_PIXEL, 0, 0, SUB_WINDOW_X_AXIS,
                0, 0, 0, 0,
                0, 0, -METER_TO_PIXEL, SUB_WINDOW_Z_AXIS;
            win.Draw2DBoundingBoxBirdsView(calibToBirdsViewMatrix * corners3DBBCamHomoCoordMatrix, "red");
            // ================================================

            // === calc the obj pos on the ground plane ===
            // Eigen::Vector3d bottomCenterPixelCoord(
            //     (tracklets[obj_i].x_2d_left + tracklets[obj_i].x_2d_right) / 2,
            //     tracklets[obj_i].y_2d_bottom,
            //     1);

            // const double theta = 0;
            // Eigen::Vector3d N(0, -std::cos(theta), std::sin(theta));

            // Eigen::Vector3d hoge = intrinsicCalibrationMatrix2D.inverse() * bottomCenterPixelCoord;
            // Eigen::Vector3d bottomCenter3DBoundingBoxGrounPlane =
            //     -HEIGHT * hoge / N.dot(hoge);

            // std::cout << bottomCenter3DBoundingBoxGrounPlane << std::endl;

            // std::cout << bottomCenter3DBoundingBoxGrounPlane << std::endl;
            // std::cout << boost::format("calc (x, y, z) = (%4d, %4d, %4d)") %
            //                  bottomCenter3DBoundingBoxGrounPlane(0) %
            //                  bottomCenter3DBoundingBoxGrounPlane(1) %
            //                  bottomCenter3DBoundingBoxGrounPlane(2)
            //           << std::endl;
            // std::cout << boost::format("true (x, y, z) = (%4d, %4d, %4d)") %
            //                  tracklets[obj_i].x_3d %
            //                  tracklets[obj_i].y_3d %
            //                  tracklets[obj_i].z_3d
            //           << std::endl;

            // Eigen::Matrix2d rotateMatrix;
            // const float angle_rad = -tracklets[obj_i].yaw_3d + M_PI / 2;
            // rotateMatrix << std::cos(angle_rad), -std::sin(angle_rad),
            //     std::sin(angle_rad), std::cos(angle_rad);
            // Eigen::Vector2d objSize(tracklets[obj_i].w_3d / 2, tracklets[obj_i].l_3d / 2);
            // Eigen::Vector2d huga = rotateMatrix.inverse() * objSize;
            // win.DrawBoundingBox(bottomCenter3DBoundingBoxGrounPlane(0) + huga(0),
            //                     bottomCenter3DBoundingBoxGrounPlane(2) + huga(1),
            //                     tracklets[obj_i].w_3d,
            //                     tracklets[obj_i].l_3d,
            //                     tracklets[obj_i].yaw_3d,
            //                     "green");
            // huga = rotateMatrix * objSize;

            // win.DrawBoundingBox(bottomCenter3DBoundingBoxGrounPlane(0) + huga(0),
            //                     bottomCenter3DBoundingBoxGrounPlane(2) + huga(1),
            //                     tracklets[obj_i].w_3d,
            //                     tracklets[obj_i].l_3d,
            //                     tracklets[obj_i].yaw_3d,
            //                     "blue");
            // ==============================================

            // === calc 2d bounding box on the image plane ===
            // Eigen::Vector4d objCenterCamCoord(tracklets[obj_i].x_3d, tracklets[obj_i].y_3d,
            //                                   tracklets[obj_i].z_3d, 1);

            // Eigen::Vector3d objCenterPixelCoord = CalibrationMatrix3D * objCenterCamCoord;
            // std::cout << boost::format("calc (x/f, y/f, 1) = (%4d, %4d, %4d)") %
            //                  (objCenterPixelCoord(0) / objCenterCamCoord(2)) %
            //                  (objCenterPixelCoord(1) / objCenterCamCoord(2)) %
            //                  1
            //           << std::endl;
            // std::cout << boost::format("true (x, y, 1) = (%4d, %4d, %4d)") %
            //                  (tracklets[obj_i].x_2d_left / 2 + tracklets[obj_i].x_2d_right / 2) %
            //                  (tracklets[obj_i].y_2d_top / 2 + tracklets[obj_i].y_2d_bottom / 2) %
            //                  1
            //           << std::endl;
            // ===============================================

            /*
            const int bottomX = (tracklets[obj_i].x_2d_left + tracklets[obj_i].x_2d_right) / 2 - 1242 / 2;
            const int bottomY = -tracklets[obj_i].y_2d_bottom + 375 / 2;
            // calc back projected bb on the groun plane from bb on the image plane
            Eigen::Vector3d centorBottom2DBoundingBoxImagePlane(
                bottomX,
                bottomY,
                1);
            // std::cout << centorBottom2DBoundingBoxImagePlane << std::endl;
            // the calibration ground plane
            const double theta = -0.03;
            Eigen::Vector3d N(0, std::cos(theta), std::sin(theta));
            // std::cout << N << std::endl;

            Eigen::Vector3d centorBottom3DBoundingBoxGrounPlane =
                -HEIGHT / N.dot(centorBottom2DBoundingBoxImagePlane) * centorBottom2DBoundingBoxImagePlane;
            std::cout << centorBottom3DBoundingBoxGrounPlane << std::endl;
            */
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
