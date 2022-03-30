cmake_minimum_required(VERSION 3.5)

project(boost_base LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)

set(BOOST_ROOT "/mnt/disk2/workspace/coding/boost_1_78_0/boost-1.78.0_install/" CACHE PATH "Boost library path")
set(Boost_NO_SYSTEM_PATHS on CACHE BOOL "Do not search system for Boost")

find_package(Boost 1.78.0 REQUIRED COMPONENTS program_options system thread coroutine)
find_package(OpenSSL REQUIRED)

add_library(${PROJECT_NAME})

file(GLOB_RECURSE PRJ_SRCS src/*.c src/*.cpp src/*.h src/*.hpp)

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_ASIO_NO_DEPRECATED=1)
target_compile_options(${PROJECT_NAME} PUBLIC "-fcoroutines")

target_sources(${PROJECT_NAME} PRIVATE ${PRJ_SRCS})

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} rt)
target_include_directories(${PROJECT_NAME} PUBLIC src/)
