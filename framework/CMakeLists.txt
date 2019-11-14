cmake_minimum_required (VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(le)
add_subdirectory(gatt)

install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bluetooth-le/framework/
  FILES_MATCHING PATTERN "*.h"
  PATTERN "*.hpp"
)
