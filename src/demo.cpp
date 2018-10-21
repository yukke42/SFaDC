/*
    demo.cpp
*/

#include <vector>

#include <opencv2/opencv.hpp>

#include "label_parser.hpp"
#include "window.hpp"

int main()
{
    std::vector<Tracklet> tracklets;
    tracklets = ParseLabelFile("../../mini_datasets/training/label_02/0000.txt");

    Window win;

    unsigned int image_id = 0;
    const unsigned int imageLast = tracklets[tracklets.size() - 1].image_id;
    while (1)
    {
        /*
            1. read an images and crate a main window
            2. create a sub window (show Bounding Boxes of objects from the bird's eye view)
            3. draw obj pos to the sub window
            4. concat them
            5. show the window
            6. wait a keybord input
        */

        win.ReadImage(0, image_id);
        win.InitSubWindow();
        win.PutImageIdText(image_id, imageLast);

        for (unsigned int obj_i = 1; obj_i < tracklets.size(); obj_i++)
        {
            if (tracklets[obj_i].image_id != image_id)
                continue;
            if (tracklets[obj_i].obj_type == "DontCare")
                continue;

            win.Rectangle(tracklets[obj_i].x_3d,
                          tracklets[obj_i].z_3d,
                          tracklets[obj_i].w_3d,
                          tracklets[obj_i].l_3d);
        }

        win.Concat();
        win.Show();

        int pressed_key = win.WaitKey();
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
