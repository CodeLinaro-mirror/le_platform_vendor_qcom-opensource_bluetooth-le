cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

project(le-gatt-service)

add_library(
  ${PROJECT_NAME}   STATIC
  ${CMAKE_SOURCE_DIR}/service/src/AdvertiseHelper.cpp
  ${CMAKE_SOURCE_DIR}/service/src/AdvertiserManager.cpp
  ${CMAKE_SOURCE_DIR}/service/src/AdvtFilterOnFoundOnLostInfo.cpp

  ${CMAKE_SOURCE_DIR}/service/src/CallbackInfo.cpp
  ${CMAKE_SOURCE_DIR}/service/src/FilterParams.cpp
  ${CMAKE_SOURCE_DIR}/service/src/GattDbElement.cpp
  ${CMAKE_SOURCE_DIR}/service/src/GattLibService.cpp
  ${CMAKE_SOURCE_DIR}/service/src/HandleMap.cpp
  ${CMAKE_SOURCE_DIR}/service/src/PeriodicScanManager.cpp
  ${CMAKE_SOURCE_DIR}/service/src/ScanClient.cpp
  ${CMAKE_SOURCE_DIR}/service/src/ScanFilterQueue.cpp
  ${CMAKE_SOURCE_DIR}/service/src/ScanManager.cpp

  ${CMAKE_SOURCE_DIR}/service/ipc/GattClientManager.cpp
  ${CMAKE_SOURCE_DIR}/service/ipc/GattClientListenerClient.cpp
  ${CMAKE_SOURCE_DIR}/service/ipc/GattServerListenerClient.cpp
  ${CMAKE_SOURCE_DIR}/service/ipc/GattServerManager.cpp
  ${CMAKE_SOURCE_DIR}/service/ipc/GattDBusService.cpp
  ${CMAKE_SOURCE_DIR}/service/ipc/GattLeAdvertiserManager.cpp
  ${CMAKE_SOURCE_DIR}/service/ipc/GattLeAdvertising.cpp
  ${CMAKE_SOURCE_DIR}/service/ipc/GattLeScannerManager.cpp
  ${CMAKE_SOURCE_DIR}/service/ipc/GattLeScanner.cpp
)

link_directories(
  ${LE_GATT_UTILS_LIBRARY_DIRS}
)

target_link_libraries(
  ${PROJECT_NAME}
  le-gatt-utils
  systemd
)

target_include_directories(
  ${PROJECT_NAME}   PUBLIC
   ${CMAKE_SOURCE_DIR}/utils/include
   ${CMAKE_SOURCE_DIR}/service/include
   ${CMAKE_SOURCE_DIR}/service/ipc
   ${CMAKE_SOURCE_DIR}/framework/include
   ${CMAKE_SOURCE_DIR}/framework/ipc
   ${CMAKE_SOURCE_DIR}/native
   ${CMAKE_SOURCE_DIR}/
   ${LIBCHROME_INCLUDE_DIRS}
   ${INCLUDES_SYSROOT}/fluoride
   ${INCLUDES_SYSROOT}/fluoride/types
  )

install(
  TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_file(
  le-gatt-service.pc.in
  le-gatt-service.pc
  @ONLY
)

install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bluetooth-le/service/
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "*.hpp"
)
install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/native/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bluetooth-le/native/
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "*.hpp"
)
