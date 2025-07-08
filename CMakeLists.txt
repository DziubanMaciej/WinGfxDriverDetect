cmake_minimum_required(VERSION 3.10.0)
project(GetDriverStore)

if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

find_package(OpenGL REQUIRED)

add_executable(GetDriverStore main.cpp)
target_link_libraries(GetDriverStore OpenGL::GL)
target_compile_features(GetDriverStore PRIVATE cxx_std_17)
