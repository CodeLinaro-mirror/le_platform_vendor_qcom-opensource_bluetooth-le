cmake_minimum_required (VERSION 3.1)

project(ble_gengattlib)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -std=c++11 -g -fpermissive -D__GLIBC__ -Wl,--no-as-needed -lm -ldl -lrt -lpthread -lbttypes -lchrome -llog -fPIC -lbtvendorhal -lcutils ")


set(INCLUDES_SYSROOT "" CACHE STRING "Path to the includes sysroot")

option(WITH_TESTS "Enable building and automatic running of unit tests" ON)

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCHROME libchrome REQUIRED)

include_directories(
  ${CMAKE_SOURCE_DIR}/utils/include
  ${CMAKE_SOURCE_DIR}/framework/include
  ${CMAKE_SOURCE_DIR}/service/include
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/

  )

add_subdirectory(utils)
add_subdirectory(service)
add_subdirectory(framework)

if(WITH_TESTS)
  add_subdirectory(test)
endif()

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/framework/le/le-framework.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/framework/gatt/gatt-framework.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/service/le-gatt-service.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bluetooth-le/
  FILES_MATCHING PATTERN "*.h"
  PATTERN "*.hpp"
)

