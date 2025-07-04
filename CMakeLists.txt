cmake_minimum_required(VERSION 4.0)
project(WardenDaemon)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES "Source/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")


set(HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Utils/config.h
)

add_executable(${PROJECT_NAME} ${HEADER} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC Source)

# DOWNLOAD dependency
include(FetchContent)

# 1. notify-cpp
FetchContent_Declare(notify-cpp GIT_REPOSITORY "https://github.com/Skiftsu/notify-cpp.git" GIT_TAG main)
set(ENABLE_SHARED_LIBS OFF CACHE BOOL "Disable shared libs" FORCE)
set(ENABLE_STATIC_LIBS ON CACHE BOOL "Enable static libs" FORCE)
FetchContent_MakeAvailable(notify-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE notify-cpp-static)

# 2. sdbus-cpp
FetchContent_Declare(sdbus-cpp GIT_REPOSITORY "https://github.com/Kistler-Group/sdbus-cpp.git")
FetchContent_MakeAvailable(sdbus-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE sdbus-c++)

# 3. json
FetchContent_Declare(json GIT_REPOSITORY "https://github.com/nlohmann/json.git")
FetchContent_MakeAvailable(json)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json)