cmake_minimum_required( VERSION 3.7 )
project( demo )

set(CMAKE_CXX_FLAGS "-O2 -Wall -std=c++17")

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( demo demo.cpp window.cpp label_parser.cpp utils.cpp)

target_link_libraries( demo ${OpenCV_LIBS} )
