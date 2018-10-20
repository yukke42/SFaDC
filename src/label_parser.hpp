/*
Values    Name      Description
----------------------------------------------------------------------------
   1    frame        Frame within the sequence where the object appearers
   1    track id     Unique tracking id of this object within this sequence
   1    type         Describes the type of object: 'Car', 'Van', 'Truck',
                     'Pedestrian', 'Person_sitting', 'Cyclist', 'Tram',
                     'Misc' or 'DontCare'
   1    truncated    Integer (0,1,2) indicating the level of truncation.
                     Note that this is in contrast to the object detection
                     benchmark where truncation is a float in [0,1].
   1    occluded     Integer (0,1,2,3) indicating occlusion state:
                     0 = fully visible, 1 = partly occluded
                     2 = largely occluded, 3 = unknown
   1    alpha        Observation angle of object, ranging [-pi..pi]
   4    bbox         2D bounding box of object in the image (0-based index):
                     contains left, top, right, bottom pixel coordinates
   3    dimensions   3D object dimensions: height, width, length (in meters)
   3    location     3D object location x,y,z in camera coordinates (in meters)
   1    rotation_y   Rotation ry around Y-axis in camera coordinates [-pi..pi]
   1    score        Only for results: Float, indicating confidence in
                     detection, needed for p/r curves, higher is better.
*/

#ifndef LABEL_PARSER_HPP
#define LABEL_PARSER_HPP

#include <string>
#include <vector>

struct Tracklet
{
    int frame_no;
    int track_id;
    std::string obj_type;
    int truncation;
    int occlusion;
    float alpha;

    float x_2d_left;
    float x_2d_right;
    float y_2d_top;
    float y_2d_bottom;

    float h_3d;
    float w_3d;
    float l_3d;
    float x_3d;
    float y_3d;
    float z_3d;
    float yaw_3d;

    float score;
};

std::vector<Tracklet> parse_label_file(std::string);

#endif