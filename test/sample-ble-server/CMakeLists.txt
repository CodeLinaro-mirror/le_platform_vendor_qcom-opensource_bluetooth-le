cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

project(sample-ble-server)

add_executable(
  ${PROJECT_NAME}
  ${PROJECT_SOURCE_DIR}/src/SampleBleServer.cpp
)

target_include_directories(
  ${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/framework/include
  )

target_link_libraries(
   ${PROJECT_NAME}
   le-framework
   gatt-framework
)

install(
  TARGETS ${PROJECT_NAME} RUNTIME
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

