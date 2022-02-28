cmake_minimum_required(VERSION 3.5)

project(boost_base LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)

find_package(Boost REQUIRED COMPONENTS program_options system thread)
find_package(OpenSSL REQUIRED)

add_library(${PROJECT_NAME})

file(GLOB_RECURSE PRJ_SRCS src/*.c src/*.cpp src/*.h src/*.hpp)

target_sources(${PROJECT_NAME} PRIVATE ${PRJ_SRCS})

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENSSL_LIBRARIES} Boost::program_options Boost::system Boost::thread rt)
target_include_directories(${PROJECT_NAME} PUBLIC src/)
