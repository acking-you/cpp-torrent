cmake_minimum_required(VERSION 3.16)
#你的项目名称
project(cpp-torrent)

set(CMAKE_CXX_STANDARD 20)
find_package(catch2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
file (GLOB_RECURSE files src/*.cpp)

add_executable(cpp-torrent ${files})
target_link_libraries(cpp-torrent PRIVATE  Catch2::Catch2 PRIVATE nlohmann_json::nlohmann_json)

