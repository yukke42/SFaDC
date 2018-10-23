/*
    demo.cpp

    image set 0: [1242 x 375]
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
    Eigen::MatrixXd calibrationMatrix3D = ParseCalibFile(calibFilePath);

    std::cout << calibrationMatrix3D << std::endl;
    // std::cout << CalibrationMatrix.rows() << std::endl;
    // std::cout << CalibrationMatrix.cols() << std::endl;
    // std::cout << CalibrationMatrix(0, 0) << std::endl;
    Eigen::MatrixXd intrinsicCalibrationMatrix2D(3, 3);
    intrinsicCalibrationMatrix2D << calibrationMatrix3D(0, 0), calibrationMatrix3D(0, 1), calibrationMatrix3D(0, 2),
        calibrationMatrix3D(1, 0), calibrationMatrix3D(1, 1), calibrationMatrix3D(1, 2),
        calibrationMatrix3D(2, 0), calibrationMatrix3D(2, 1), calibrationMatrix3D(2, 2);
    // std::cout << CalibrationMatrix2D << std::endl;
    // std::cout << CalibrationMatrix2D.inverse() << std::endl;

    Window win;

    unsigned int image_id = 0;
    const unsigned int imageLast = tracklets[tracklets.size() - 1].image_id;
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

        std::string imageFilePath = str(boost::format("%s/image_02/%04d/%06d.png") % dataSetsRootDir % imageSetsId % image_id);

        win.ReadImage(imageFilePath);
        win.InitSubWindow();
        win.PutImageIdText(image_id, imageLast);

        for (unsigned int obj_i = 1; obj_i < tracklets.size(); obj_i++)
        {
            if (tracklets[obj_i].image_id != image_id)
                continue;
            if (tracklets[obj_i].obj_type == "DontCare")
                continue;

            // === calc the obj pos on the ground plane ===
            Eigen::Vector3d bottomCenterPixelCoord(
                (tracklets[obj_i].x_2d_left + tracklets[obj_i].x_2d_right) / 2,
                tracklets[obj_i].y_2d_bottom,
                1);

            const double theta = -0.03;
            Eigen::Vector3d N(0, -std::cos(theta), std::sin(theta));

            Eigen::Vector3d bottomCenter3DBoundingBoxGrounPlane =
                -HEIGHT * intrinsicCalibrationMatrix2D.inverse() * bottomCenterPixelCoord /
                N.dot(intrinsicCalibrationMatrix2D.inverse() * bottomCenterPixelCoord);

            // std::cout << bottomCenter3DBoundingBoxGrounPlane << std::endl;
            std::cout << boost::format("calc (x, z) = (%4d, %4d)") %
                             bottomCenter3DBoundingBoxGrounPlane(0) %
                             bottomCenter3DBoundingBoxGrounPlane(2)
                      << std::endl;
            std::cout << boost::format("true (x, z) = (%4d, %4d)") %
                             tracklets[obj_i].x_3d %
                             tracklets[obj_i].z_3d
                      << std::endl;

            win.DrawBoundingBox(bottomCenter3DBoundingBoxGrounPlane(0),
                                bottomCenter3DBoundingBoxGrounPlane(2),
                                tracklets[obj_i].w_3d,
                                tracklets[obj_i].l_3d,
                                tracklets[obj_i].yaw_3d,
                                "green");
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

            win.DrawBoundingBox(tracklets[obj_i].x_3d,
                                tracklets[obj_i].z_3d,
                                tracklets[obj_i].w_3d,
                                tracklets[obj_i].l_3d,
                                tracklets[obj_i].yaw_3d,
                                "red");
        }

        win.Concat();
        win.Show();

        int pressed_key = win.WaitKey() & 0xff;
        // std::cout << "pressed key num: " << pressed_key << std::endl;
        if (pressed_key == 113) // q
        {
            break;
        }
        else if (pressed_key == 110) // n
        {
            if (image_id < imageLast)
                image_id++;
        }
        else if (pressed_key == 112) // p
        {
            if (image_id > 1)
                image_id--;
        }
    }

    return 0;
}
