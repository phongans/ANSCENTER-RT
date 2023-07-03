cmake_minimum_required(VERSION 3.17)

project(ovida-rt)

# Use ccache to speed up rebuilds
include(cmake/ccache.cmake)

# Set C++ version and optimization level
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /W4 -DGTL_STATIC" CACHE STRING COMPILE_FLAGS FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Ox /Ob2 /Oi /Ot /arch:AVX2 /fp:fast /DNDEBUG /std:c++latest" CACHE STRING COMPILE_FLAGS FORCE)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

list(APPEND CMAKE_PREFIX_PATH "C:/Qt/Qt5.12.12/5.12.12/msvc2017_64")

# TODO: Specify the path to TensorRT root dir
set(TensorRT_DIR C:/TensorRT-8.5.1.7)
include(cmake/FindTensorRT.cmake)

########
# Qt 5 #
########
set(Qt5Widgets_DIR C:/Qt/Qt5.12.5/5.12.5/msvc2017)
find_package(Qt5Widgets REQUIRED)

##########
# OpenCV #
##########
set(OpenCV_STATIC OFF)
set(OpenCV_DIR "C:/opencv")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
SET(Eigen3_DIR "C:/eigen-3.4.0/build")
find_package(Eigen3 REQUIRED)

###############
# Application #
###############
add_subdirectory(src)
