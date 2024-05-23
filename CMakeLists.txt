cmake_minimum_required(VERSION 3.25.0)
project(Letrng)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(src)

# Add main executable (print output to file)
add_executable(letrng_print_to_file main.cpp)

add_library(letrng_print_lib STATIC src/letrng.cpp)

target_link_libraries(letrng_print_to_file PRIVATE letrng_print_lib)

# Add CryptoPP as dependency
find_library(CRYPTOPP_LIBRARY NAMES libcryptopp.a)
 
if(NOT CRYPTOPP_LIBRARY)
    message(FATAL_ERROR "Could not find Crypto++ library")
endif()
 
# Add CryptoPP RSA signature verification executable with CryptoPP implementation of Letrng
add_executable(letrng_rsa_signature crypto.cpp)

add_library(letrng_rsa_lib STATIC src/letrng_crypto.cpp src/letrng.cpp)

target_link_libraries(letrng_rsa_signature PRIVATE letrng_rsa_lib)

target_link_libraries(letrng_rsa_signature PRIVATE ${CRYPTOPP_LIBRARY})