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
    Eigen::MatrixXd calibrationMatrix3D = ParseCalibFile(calibFilePath);

    // std::cout << calibrationMatrix3D << std::endl;
    // std::cout << CalibrationMatrix.rows() << std::endl;
    // std::cout << CalibrationMatrix.cols() << std::endl;
    // std::cout << CalibrationMatrix(0, 0) << std::endl;
    Eigen::MatrixXd intrinsicCalibrationMatrix2D(3, 3);
    intrinsicCalibrationMatrix2D << calibrationMatrix3D(0, 0), calibrationMatrix3D(0, 1), calibrationMatrix3D(0, 2),
        calibrationMatrix3D(1, 0), calibrationMatrix3D(1, 1), calibrationMatrix3D(1, 2),
        calibrationMatrix3D(2, 0), calibrationMatrix3D(2, 1), calibrationMatrix3D(2, 2);

    Eigen::Vector3d intrinsicT(calibrationMatrix3D(0, 3), calibrationMatrix3D(1, 3), calibrationMatrix3D(2, 3));

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
        std::cout << "Image: " << image_id << std::endl;

        std::string imageFilePath = str(boost::format("%s/image_02/%04d/%06d.png") % dataSetsRootDir % imageSetsId % image_id);

        win.ReadImage(imageFilePath);
        win.InitSubWindow();
        win.PutImageIdText(image_id, imageLast);

        for (unsigned int obj_i = 1; obj_i < tracklets.size(); obj_i++)
        {

            if (tracklets[obj_i].image_id != image_id)
                continue;
            if (tracklets[obj_i].occlusion != 0)
                continue;
            if (tracklets[obj_i].obj_type == "DontCare")
                continue;

            std::cout << tracklets[obj_i].obj_type
                      << " truncated: "
                      << tracklets[obj_i].truncation
                      << " occluded: "
                      << tracklets[obj_i].occlusion
                      << std::endl;

            // === draw the 3D BB on the image plane ==
            // TODO: occlusionで 値が0(= fullu visible) になっているにもかかわらず
            //       一部しか見えていない物体が含まれる
            //       → 3D BB を線画するとおかしな図形が含まれてしまう
            // TODO: 3D BBの線画が各フレーム毎にブレが生じているのがおかしい
            double l = tracklets[obj_i].l_3d, h = tracklets[obj_i].h_3d, w = tracklets[obj_i].w_3d;
            const std::vector<double> x_corners{l / 2, l / 2, -l / 2, -l / 2,
                                                l / 2, l / 2, -l / 2, -l / 2};
            const std::vector<double> y_corners{0, 0, 0, 0, -h, -h, -h, -h};
            const std::vector<double> z_corners{w / 2, -w / 2, -w / 2, w / 2,
                                                w / 2, -w / 2, -w / 2, w / 2};
            Eigen::MatrixXd corners3DBBObjCoordMatrix(3, 8);
            for (int i = 0; i < 8; i++)
            {
                corners3DBBObjCoordMatrix(0, i) = x_corners[i];
                corners3DBBObjCoordMatrix(1, i) = y_corners[i];
                corners3DBBObjCoordMatrix(2, i) = z_corners[i];
            }
            std::cout << "objCoord " << corners3DBBObjCoordMatrix << std::endl;
            Eigen::Matrix3d rotateYAxisMatrix;
            const double yaw = tracklets[obj_i].yaw_3d;
            rotateYAxisMatrix << std::cos(yaw), 0, std::sin(yaw),
                0, 1, 0,
                -std::sin(yaw), 0, std::cos(yaw);
            std::cout << "rorate " << rotateYAxisMatrix << std::endl;

            Eigen::MatrixXd rotatedCorners3DBBObjCoordMatrix(3, 8);
            rotatedCorners3DBBObjCoordMatrix = rotateYAxisMatrix * corners3DBBObjCoordMatrix;
            Eigen::MatrixXd corners3DBBHomoCamCoord(4, 8);
            for (int i = 0; i < 8; i++)
            {
                corners3DBBHomoCamCoord(0, i) = rotatedCorners3DBBObjCoordMatrix(0, i) + tracklets[obj_i].x_3d;
                corners3DBBHomoCamCoord(1, i) = rotatedCorners3DBBObjCoordMatrix(1, i) + tracklets[obj_i].y_3d;
                corners3DBBHomoCamCoord(2, i) = rotatedCorners3DBBObjCoordMatrix(2, i) + tracklets[obj_i].z_3d;
                corners3DBBHomoCamCoord(3, i) = 1;
            }

            // std::cout << corners3DBBObjCoord << std::endl;
            std::cout << "homo " << corners3DBBHomoCamCoord << std::endl;

            Eigen::MatrixXi corners3DBBPixCoord(3, 8);
            corners3DBBPixCoord = (calibrationMatrix3D * corners3DBBHomoCamCoord).cast<int>();
            std::cout << corners3DBBPixCoord << std::endl;
            win.Draw3DBoundingBoxOnImage(corners3DBBPixCoord);
            // ========================================

            // === draw the 2D BB on the image plane ===
            win.Draw2DBoundingBoxOnImage(
                (int)tracklets[obj_i].x_2d_left,
                (int)tracklets[obj_i].x_2d_right,
                (int)tracklets[obj_i].y_2d_top,
                (int)tracklets[obj_i].y_2d_bottom);
            // =========================================

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

            win.Draw2DBoundingBoxBirdsView(tracklets[obj_i].x_3d,
                                           tracklets[obj_i].z_3d,
                                           tracklets[obj_i].w_3d,
                                           tracklets[obj_i].l_3d,
                                           tracklets[obj_i].yaw_3d,
                                           "red");
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
