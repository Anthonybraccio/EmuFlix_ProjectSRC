# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\Emuflix_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Emuflix_autogen.dir\\ParseCache.txt"
  "Emuflix_autogen"
  )
endif()
