cmake_minimum_required( VERSION 3.7 )
project( demo )

set(CMAKE_CXX_FLAGS "-O2 -Wall -std=c++17")

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIR} )

find_package( Eigen3 REQUIRED )
include_directories( ${EIGEN3_INCLUDE_DIR} )

find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

add_executable( demo demo.cpp window.cpp label_parser.cpp utils.cpp)

target_link_libraries( demo ${OpenCV_LIBS} )
