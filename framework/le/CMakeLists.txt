cmake_minimum_required (VERSION 3.1)

add_library(
  le-framework SHARED
  ${CMAKE_SOURCE_DIR}/framework/le/AdvertiseData.cpp
  ${CMAKE_SOURCE_DIR}/framework/le/AdvertiseSettings.cpp
  ${CMAKE_SOURCE_DIR}/framework/le/AdvertisingSet.cpp
  ${CMAKE_SOURCE_DIR}/framework/le/AdvertisingSetParameters.cpp
  ${CMAKE_SOURCE_DIR}/framework/le/PeriodicAdvertiseParameters.cpp
  ${CMAKE_SOURCE_DIR}/framework/le/PeriodicAdvertisingManager.cpp
  ${CMAKE_SOURCE_DIR}/framework/le/PeriodicAdvertisingReport.cpp
  ${CMAKE_SOURCE_DIR}/framework/le/ResultStorageDescriptor.cpp
  ${CMAKE_SOURCE_DIR}/framework/le/ScanFilter.cpp
  ${CMAKE_SOURCE_DIR}/framework/le/ScanRecord.cpp
  ${CMAKE_SOURCE_DIR}/framework/le/ScanResult.cpp
  ${CMAKE_SOURCE_DIR}/framework/le/ScanSettings.cpp
  ${CMAKE_SOURCE_DIR}/framework/le/TruncatedFilter.cpp
)

target_link_libraries(
  le-framework
  le-gatt-utils
)

target_include_directories(
  le-framework PUBLIC
  ${CMAKE_SOURCE_DIR}/utils/include
  ${CMAKE_SOURCE_DIR}/framework/include
  ${CMAKE_SOURCE_DIR}/framework/ipc
  ${CMAKE_SOURCE_DIR}/native
  ${INCLUDES_SYSROOT}/fluoride
  ${INCLUDES_SYSROOT}/fluoride/types
  ${LIBCHROME_INCLUDE_DIRS}
  )

install(
  TARGETS le-framework
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_file(
  le-framework.pc.in
  le-framework.pc
  @ONLY
)
