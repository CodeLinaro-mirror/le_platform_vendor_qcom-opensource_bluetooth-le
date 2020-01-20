cmake_minimum_required (VERSION 3.1)
project(gatt-framework)

add_library(
  ${PROJECT_NAME} SHARED
  ${CMAKE_SOURCE_DIR}/framework/gatt/GattCharacteristic.cpp
  ${CMAKE_SOURCE_DIR}/framework/gatt/GattClient.cpp
  ${CMAKE_SOURCE_DIR}/framework/gatt/GattDescriptor.cpp
  ${CMAKE_SOURCE_DIR}/framework/gatt/GattDevice.cpp
  ${CMAKE_SOURCE_DIR}/framework/gatt/GattIncludedService.cpp
  ${CMAKE_SOURCE_DIR}/framework/gatt/GattLeAdvertiser.cpp
  ${CMAKE_SOURCE_DIR}/framework/gatt/GattLeScanner.cpp
  ${CMAKE_SOURCE_DIR}/framework/gatt/GattServer.cpp
  ${CMAKE_SOURCE_DIR}/framework/gatt/GattService.cpp

  ${CMAKE_SOURCE_DIR}/framework/ipc/GattServerListener.cpp
  ${CMAKE_SOURCE_DIR}/framework/ipc/GattServerManagerClient.cpp
  ${CMAKE_SOURCE_DIR}/framework/ipc/GattDBusFw.cpp
  ${CMAKE_SOURCE_DIR}/framework/ipc/GattLeAdvertiserManagerClient.cpp
  ${CMAKE_SOURCE_DIR}/framework/ipc/GattLeAdvertiserListener.cpp
  ${CMAKE_SOURCE_DIR}/framework/ipc/BluetoothLeSdbus.cpp
  ${CMAKE_SOURCE_DIR}/framework/ipc/GattClientListener.cpp
  ${CMAKE_SOURCE_DIR}/framework/ipc/GattClientManagerClient.cpp
  ${CMAKE_SOURCE_DIR}/framework/ipc/GattLeScannerManagerClient.cpp
  ${CMAKE_SOURCE_DIR}/framework/ipc/GattLeScannerListener.cpp
)

target_link_libraries(
  ${PROJECT_NAME}
  le-gatt-utils
  systemdq
)

target_include_directories(
   ${PROJECT_NAME} PUBLIC
   ${CMAKE_SOURCE_DIR}/utils/include
   ${CMAKE_SOURCE_DIR}/framework/include
   ${CMAKE_SOURCE_DIR}/framework/ipc
   ${CMAKE_SOURCE_DIR}/native
   ${INCLUDES_SYSROOT}/fluoride
   ${INCLUDES_SYSROOT}/fluoride/types
   ${LIBCHROME_INCLUDE_DIRS}
  )

install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_file(
  gatt-framework.pc.in
  gatt-framework.pc
  @ONLY
)
