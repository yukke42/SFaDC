/*
    demo.cpp
*/

#include <iomanip>
#include <iostream>
#include <vector>

#include "label_parser.hpp"
#include "window.hpp"

int main(int argc, char *argv[])
{
    // if (argc < 2)
    // {
    //     std::cout << "Usage: ./demo IMAGE_SET_ID" << std::endl;
    //     return 1;
    // }
    // const unsigned int imageSetsId = atoi(argv[1]);

    // const std::string datasetsRootDir = "../../datasets/training/";
    // std::ostringstream image_sets_id_str, image_dirpath;
    // image_sets_id_str << std::setw(4) << std::setfill('0') << imageSetsId;
    // const std::string labelFilePath = datasetsRootDir + "label_02/" + image_sets_id_str.str() + ".txt";
    // const std::string imagesetsDirPath = datasetsRootDir + "image_02/" + image_sets_id_str.str() + "/";
    std::vector<Tracklet> tracklets = ParseLabelFile("../../datasets/training/label_02/0000.txt");

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

        std::ostringstream image_id_str;
        std::string imagesetsDirPath = "../../datasets/training/image_02/0000/";
        image_id_str << std::setw(6) << std::setfill('0') << image_id;
        const std::string imageFilePath = imagesetsDirPath + image_id_str.str() + ".png";

        win.ReadImage(imageFilePath);
        win.InitSubWindow();
        win.PutImageIdText(image_id, imageLast);

        for (unsigned int obj_i = 1; obj_i < tracklets.size(); obj_i++)
        {
            if (tracklets[obj_i].image_id != image_id)
                continue;
            if (tracklets[obj_i].obj_type == "DontCare")
                continue;

            std::cout << tracklets[obj_i].obj_type << ": " << tracklets[obj_i].yaw_3d << std::endl;

            win.DrawBoundingBox(tracklets[obj_i].x_3d,
                                tracklets[obj_i].z_3d,
                                tracklets[obj_i].w_3d,
                                tracklets[obj_i].l_3d,
                                tracklets[obj_i].yaw_3d);
        }

        win.Concat();
        win.Show();

        int pressed_key = win.WaitKey() & 0xff;
        std::cout << "pressed key num: " << pressed_key << std::endl;
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
