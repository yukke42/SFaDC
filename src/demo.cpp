/*
demo.cpp
*/

#include <iostream>
#include <vector>

#include "label_parser.hpp"
#include "window.hpp"

int main()
{
    std::vector<Tracklet> tracklets;
    tracklets = ParseLabelFile("../../mini_datasets/training/label_02/0000.txt");

    Window window;

    int frame_no = 0;
    const int frameLast = tracklets[tracklets.size() - 1].frame_no;
    while (1)
    {
        window.PutFrameNoText(frame_no, frameLast);

        for (unsigned int obj_i = 1; obj_i < tracklets.size(); obj_i++)
        {
            if (tracklets[obj_i].frame_no != frame_no)
                continue;
            if (tracklets[obj_i].obj_type == "DontCare")
                continue;

            window.Rectangle(tracklets[obj_i].x_3d,
                             tracklets[obj_i].z_3d,
                             tracklets[obj_i].w_3d,
                             tracklets[obj_i].l_3d);
        }

        window.Show();

        int pressed_key = window.WaitKey();
        if (pressed_key == 113) // q
        {
            break;
        }
        else if (pressed_key == 110) // n
        {
            if (frame_no < frameLast)
                frame_no++;
        }
        else if (pressed_key == 112) // p
        {
            if (frame_no > 1)
                frame_no--;
        }

        window.Refresh();
    }

    return 0;
}
