cmake_minimum_required(VERSION 3.10.0)
project(GetDriverStore)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

add_executable(GetDriverStore main.cpp)
target_link_libraries(GetDriverStore opengl32.lib)
