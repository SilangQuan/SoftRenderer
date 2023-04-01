cmake_minimum_required(VERSION 3.0.0)
project(SoftRenderer VERSION 0.0.1)

include(CTest)
enable_testing()

INCLUDE_DIRECTORIES(${SoftRenderer_SOURCE_DIR})  
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/3rdParty)

file(GLOB ALL_SRC
    "*.h"
    "*.cpp"
	"*.hpp"
    "Base/*.cpp"
    "Light/*.cpp"
    "Log/*.cpp"
    "Math/*.cpp"
    "Shape/*.cpp"
    "RenderSystem/*.cpp"
    "Profiler/*.cpp"
)

add_executable(SoftRenderer ${ALL_SRC})

if(UNIX)
    #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=gnu++0x")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=gnu++0x")
endif()
find_library(SDL_LIBRARY SDL2 ${PROJECT_SOURCE_DIR}/3rdParty/SDL2/lib)
find_library(SDL_MAIN_LIBRARY SDL2main ${PROJECT_SOURCE_DIR}/3rdParty/SDL2/lib)
if(SDL_LIBRARY)
  target_link_libraries(SoftRenderer ${SDL_LIBRARY})
endif()
if(SDL_MAIN_LIBRARY)
  target_link_libraries(SoftRenderer ${SDL_MAIN_LIBRARY})
endif()

#link_directories(${PROJECT_SOURCE_DIR}/3rdParty/SDL2/lib)
#TARGET_LINK_LIBRARIES(SoftRenderer SDL2 SDL2main) 

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
