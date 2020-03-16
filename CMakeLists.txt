cmake_minimum_required(VERSION 2.8.3)
project(imguicmake)
find_package(Eigen3 REQUIRED)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )

ADD_SUBDIRECTORY(GUI)
ADD_SUBDIRECTORY(GUI3D)

add_executable(exe exe.cpp )
target_link_libraries(exe PUBLIC GUI)