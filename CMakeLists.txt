cmake_minimum_required(VERSION 3.26)
project(20240506_DerivativesPricing)

set(CMAKE_CXX_STANDARD 17)

add_executable(20240506_DerivativesPricing main.cpp)

include_directories(include) # include
file(GLOB SOURCES src/*.cpp) # src
file(GLOB MAINS mains/*.cpp) # mains
foreach(MAIN ${MAINS})
    get_filename_component(MAIN_NAME ${MAIN} NAME_WE) # main name
    add_executable(${MAIN_NAME} ${SOURCES} ${MAIN}) # main exe
endforeach()

