/*
demo.cpp
*/

#include "demo.hpp"

#include <iostream>
#include <vector>


#include "label_parser.hpp"
#include "window.hpp"

int main()
{
    std::vector<Tracklet> tracklets;
    tracklets = parse_label_file("../../mini_datasets/training/label_02/0000.txt");

    Window window;

    for (unsigned int obj_i = 1; obj_i < tracklets.size(); obj_i++)
    {
        if (tracklets[obj_i].obj_type != "Car")
            continue;
        if (tracklets[obj_i].occlusion != 0)
            continue;

        window.rectangle(tracklets[obj_i].x_3d,
                         tracklets[obj_i].z_3d,
                         tracklets[obj_i].w_3d,
                         tracklets[obj_i].l_3d);

        std::cout << tracklets[obj_i].h_3d << std::endl;
        std::cout << tracklets[obj_i].w_3d << std::endl;
        std::cout << tracklets[obj_i].l_3d << std::endl;
        std::cout << tracklets[obj_i].x_3d << std::endl;
        std::cout << tracklets[obj_i].y_3d << std::endl;
        std::cout << tracklets[obj_i].z_3d << std::endl;

        break;
    }

    window.show();

    return 0;
}
