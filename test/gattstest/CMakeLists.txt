cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

project(bluetooth-le-gatt-server-test)

add_executable(
  ${PROJECT_NAME}
  ${PROJECT_SOURCE_DIR}/src/bluetooth-le-gatt-server-test.cpp
  ${PROJECT_SOURCE_DIR}/src/gatt-server-test-service.cpp
  ${PROJECT_SOURCE_DIR}/src/GattsTest.cpp
)

target_include_directories(
  ${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/
  ${PROJECT_SOURCE_DIR}/include
  ${LIBCHROME_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/utils/include
  ${CMAKE_SOURCE_DIR}/test/utils/include
  ${CMAKE_SOURCE_DIR}/framework/include
  ${CMAKE_SOURCE_DIR}/service/include
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/
  )

target_link_libraries(
   ${PROJECT_NAME}
   ${LIBCHROME_LIBRARIES}
   le-framework
   gatt-framework
   le-gatt-service
   le-gatt-utils
   bluetooth-le-test-utils
   bttypes
   pthread
)

install(
  TARGETS ${PROJECT_NAME} RUNTIME
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

