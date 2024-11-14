cmake_minimum_required(VERSION 3.12) # FetchContent is available in 3.11+
# use vcpkg to find library
set(CMAKE_TOOLCHAIN_FILE D:/vcpkg/scripts/buildsystems/vcpkg.cmake)
include(FetchContent)

project(Doita)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG ae50bfa2cc569c0f8d5bc4315d39db64005b1b08 
)
FetchContent_MakeAvailable(raylib)
include_directories("${raylib_SOURCE_DIR}/src")

FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG 25c8c65a6e5f0f4d4b564a0343861898c6f2778b 
)
FetchContent_MakeAvailable(raygui)
include_directories("${raygui_SOURCE_DIR}/src")

# include all files in /src
file (GLOB project_SRC
	"src/*.h"
	"src/*.cpp"
	)

file(COPY assets DESTINATION "${CMAKE_BINARY_DIR}/Debug")

# Our Project
add_executable(${PROJECT_NAME} ${project_SRC})
target_link_libraries(${PROJECT_NAME} raylib)