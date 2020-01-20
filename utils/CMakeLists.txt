cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

project(le-gatt-utils)

add_library(
  ${PROJECT_NAME} SHARED
  ${PROJECT_SOURCE_DIR}/src/uuid.cc
)

target_include_directories(
  ${PROJECT_NAME}  PUBLIC
  ${CMAKE_SOURCE_DIR}/utils/include
  ${LIBCHROME_INCLUDE_DIRS}
  ${INCLUDES_SYSROOT}/fluoride
  ${INCLUDES_SYSROOT}/fluoride/types
  ${PROJECT_SOURCE_DIR}/include
  )

install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}
)

configure_file(
  le-gatt-utils.pc.in
  le-gatt-utils.pc
  @ONLY
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/le-gatt-utils.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bluetooth-le/utils/include/
  FILES_MATCHING PATTERN "*.h"
                 PATTERN "*.hpp"
)
