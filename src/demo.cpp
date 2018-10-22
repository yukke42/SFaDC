/*
    demo.cpp
*/

#include "boost/format.hpp"

#include <iostream>
#include <vector>

#include "label_parser.hpp"
#include "window.hpp"

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
