# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\lab8_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\lab8_autogen.dir\\ParseCache.txt"
  "lab8_autogen"
  )
endif()
