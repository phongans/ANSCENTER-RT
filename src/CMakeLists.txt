set(inference_headers
	Plugins/inference/src/inference_trt.h
)

file(GLOB_RECURSE UI_FILES *.ui)
file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)

qt5_wrap_ui(UI_HEADERS ${UI_FILES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/Config.h
)

find_library(TRT_API_LIBRARY NAMES tensorrt_cpp_api PATHS C:/Projects/OVIDA-RT/src/Plugins/Inference/libtensorrt/build/Release)
find_library(DETECTOR_LIBRARY NAMES inferencetrt PATHS C:/Projects/OVIDA-RT/src/Plugins/Inference/build/Release)

add_executable(${CMAKE_PROJECT_NAME}
  ${UI_HEADERS}
  ${SOURCE_FILES}
  ${HEADER_FILES}
  ${inference_headers}
)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
  Qt5::Widgets
  ${OpenCV_LIBS}
  ${TRT_API_LIBRARY}
  ${DETECTOR_LIBRARY}
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${inference_headers})