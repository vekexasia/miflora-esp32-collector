# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16.0)
add_definitions(-DARDUINO=100 -DARDUINO_ARCH_ESP32)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(project-name)