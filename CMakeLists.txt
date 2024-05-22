cmake_minimum_required(VERSION 3.25.0)
project(Letrng)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Letrng main.cpp)

include_directories(src)

add_library(LetrngLib STATIC src/letrng.cpp)

target_link_libraries(Letrng PRIVATE LetrngLib)
