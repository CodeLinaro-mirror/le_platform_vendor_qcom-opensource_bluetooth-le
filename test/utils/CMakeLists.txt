cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

project(bluetooth-le-test-utils)

find_package (Threads)

add_library(
  ${PROJECT_NAME} STATIC
  ${PROJECT_SOURCE_DIR}/src/common-test-cli.cc
)

target_include_directories(
  ${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/include
  )
