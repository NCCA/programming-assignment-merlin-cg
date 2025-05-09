# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/ParticleQt_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/ParticleQt_autogen.dir/ParseCache.txt"
  "ParticleQt_autogen"
  )
endif()
