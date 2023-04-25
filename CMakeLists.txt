cmake_minimum_required(VERSION 3.17)

project(anscenter-desktop)

set(PROJECT_NAME            "anscenter-desktop")
set(PROJECT_VERSION         "1.0.0")
set(PROJECT_AUTHOR_NAME     "Pham Thanh Phong")
set(PROJECT_AUTHOR_EMAIL    "phongpham663@gmail.com")
set(PROJECT_AUTHOR_WEBSITE  "www.anscenter.com")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

########
# Qt 5 #
########
find_package(Qt5Widgets REQUIRED)

##########
# OpenCV #
##########
set(OpenCV_STATIC OFF)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

###############
# Application #
###############
add_subdirectory(src)