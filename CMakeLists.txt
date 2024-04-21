cmake_minimum_required(VERSION 3.29.2)
project(LETRNG VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(SOURCES "src/main.cpp" "src/letrng.cpp" "src/letrng.h")
add_executable(LETRNG ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
