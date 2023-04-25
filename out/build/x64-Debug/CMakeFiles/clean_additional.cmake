# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "src\\CMakeFiles\\anscenter-desktop_autogen.dir\\AutogenUsed.txt"
  "src\\CMakeFiles\\anscenter-desktop_autogen.dir\\ParseCache.txt"
  "src\\anscenter-desktop_autogen"
  )
endif()
