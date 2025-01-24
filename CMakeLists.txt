cmake_minimum_required(VERSION 3.25)

project(
    applejuice
)

set(CMAKE_BUILD_TYPE Debug)
set(ROOT_DIR ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 20)

# add_definitions( -DSUPPORT_FILEFORMAT_PNG )

set(RAYLIB_DIR raylib)
add_subdirectory(${RAYLIB_DIR})

include_directories(external)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/code/game.cpp
)

add_executable(${PROJECT_NAME} 
    ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} 
	raylib
)
