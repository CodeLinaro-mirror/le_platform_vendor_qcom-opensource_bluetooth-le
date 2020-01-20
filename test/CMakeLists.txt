cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

project(bluetooth-le-test)

add_subdirectory(utils)
#add_subdirectory(service-stub)
add_subdirectory(gattctest)
add_subdirectory(gattstest)
add_subdirectory(sample-ble-server)


