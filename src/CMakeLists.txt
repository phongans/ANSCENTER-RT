set(inference_headers
	Plugins/inference/include/inference_trt.h
	Plugins/Tracker/include/OCSort.hpp
)

file(GLOB_RECURSE UI_FILES *.ui)
file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)

qt5_wrap_ui(UI_HEADERS ${UI_FILES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/Config.h
)

find_library(TRT_API_LIBRARY NAMES tensorrt_cpp_api PATHS Plugins/Inference/libtensorrt/lib)
find_library(DETECTOR_LIBRARY NAMES inferencetrt PATHS Plugins/Inference/lib)
find_library(OCSORT_LIBRARY NAMES ocsort PATHS Plugins/Tracker/lib)

include_directories(Plugins/Inference/include)
include_directories(Plugins/Inference/libtensorrt/include)
include_directories(Plugins/Tracker/include)

add_executable(${CMAKE_PROJECT_NAME}
  ${UI_HEADERS}
  ${SOURCE_FILES}
  ${HEADER_FILES}
  ${inference_headers}
)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
  Qt5::Widgets
  Eigen3::Eigen
  ${OpenCV_LIBS}
  ${CUDA_LIBRARIES}
  ${TensorRT_LIBRARIES}
  ${TRT_API_LIBRARY}
  ${DETECTOR_LIBRARY}
  ${OCSORT_LIBRARY}
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${inference_headers} ${TensorRT_INCLUDE_DIR} ${CUDA_TOOLKIT_INCLUDE})