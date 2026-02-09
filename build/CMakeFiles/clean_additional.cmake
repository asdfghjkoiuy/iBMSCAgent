# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/iBMSCQt_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/iBMSCQt_autogen.dir/ParseCache.txt"
  "iBMSCQt_autogen"
  )
endif()
