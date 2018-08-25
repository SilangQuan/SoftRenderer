cmake_minimum_required(VERSION 3.0.0)
project(SoftRenderer VERSION 0.0.0)

include(CTest)
enable_testing()

INCLUDE_DIRECTORIES(${SoftRenderer_SOURCE_DIR})  

file(GLOB ALL_SRC
    "*.h"
    "*.cpp"
    "Base/*.cpp"
    "Light/*.cpp"
    "Log/*.cpp"
    "Math/*.cpp"
    "Shape/*.cpp"
    "RenderSystem/*.cpp"
)

add_executable(SoftRenderer ${ALL_SRC})

if(UNIX)
    #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=gnu++0x")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=gnu++0x")
endif()

TARGET_LINK_LIBRARIES(SoftRenderer SDL2 GL GLU) 

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
