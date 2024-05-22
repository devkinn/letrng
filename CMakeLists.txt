cmake_minimum_required(VERSION 3.25.0)
project(Letrng)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Letrng main.cpp)

include_directories(src)

add_library(LetrngLib STATIC src/letrng.cpp)

target_link_libraries(Letrng PRIVATE LetrngLib)

# Add CryptoPP as dependency
# find_library(CRYPTOPP_LIBRARY NAMES libcryptopp.a)
# 
# if(NOT CRYPTOPP_LIBRARY)
#     message(FATAL_ERROR "Could not find Crypto++ library")
# endif()
# 
# target_link_libraries(Letrng PRIVATE ${CRYPTOPP_LIBRARY})