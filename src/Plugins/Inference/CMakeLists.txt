cmake_minimum_required(VERSION 3.10)
project(inferencetrt)

unset(CMAKE_C_FLAGS CACHE)
unset(CMAKE_CXX_FLAGS CACHE)
unset(CMAKE_CXX_FLAGS_RELEASE CACHE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /W4 -DGTL_STATIC" CACHE STRING COMPILE_FLAGS FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Ox /Ob2 /Oi /Ot /arch:AVX2 /fp:fast /DNDEBUG /std:c++latest" CACHE STRING COMPILE_FLAGS FORCE)

# Use ccache to speed up rebuilds
include(cmake/ccache.cmake)
# Linker external library
set(OpenCV_DIR "C:/opencv")
find_package(OpenCV REQUIRED)

set(inference_sources
	src/inference_trt.cpp
	libtensorrt/src/engine.cpp
)

set(inference_headers
	src/inference_trt.h
	libtensorrt/src/engine.h
)

# TODO: Specify the path to TensorRT root dir
set(TensorRT_DIR C:/TensorRT-8.5.1.7)

# Build the inferenceTRT library
add_library(${PROJECT_NAME} SHARED ${inference_sources})
target_link_libraries(${PROJECT_NAME} PUBLIC tensorrt_cpp_api ${OpenCV_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC libtensorrt/src)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${tracker_headers}")